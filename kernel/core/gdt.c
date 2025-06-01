#include "../inc/gdt.h"
#include "../inc/mem.h"

// Mira Kernel GDT Set Entry
static mk_gdt_entry_t  mk_gdt[MK_GDT_NUM_ENTRIES] __attribute__((aligned(0x10)));

// Mira Kernel TSS & Stack
mk_tss_t mk_tss __attribute__((aligned(0x10))); // Non-static for scheduler to access
static uint8_t mk_stack0[4096] __attribute__((aligned(0x10)));

// Mira Kernel GDT Pointer
static mk_gdt_ptr_t mk_gdt_descriptor __attribute__((aligned(0x10)));

// Mira Kernel GDT Set Segment
static void mk_gdt_set_seg(mk_gdt_entry_t *entry, uint8_t dpl, int is_code) {
    // limit = 0, base = 0
    entry->limit_low = 0x0000;
    entry->base_low = 0x0000;
    entry->base_mid = 0x00;

    if (is_code) {
        entry->access = (uint8_t)(0x9A | (dpl << 5)); // 0x9A=code,r0 or 0xFA=r3
        entry->granularity = 0x20;  // L=1 (64-bit code), G=0 is fine
    } else {
        entry->access = (uint8_t)(0x92 | (dpl << 5)); // 0x92=data,r0 or 0xF2=r3
        entry->granularity = 0x00;  // data segments in long mode ignore limit
    }

    entry->base_high = 0x00;
}

// Mira Kernel GDT Set TSS
// Writes a 16-byte TSS descriptor (mk_tss at address `base`) into GDT slots [5] and [6].
static void mk_gdt_set_tss(mk_tss_desc_t *desc, uintptr_t base, uint32_t limit) {
    desc->length = (uint16_t)(limit & 0xFFFF);
    desc->base_low = (uint16_t)(base & 0xFFFF);
    desc->base_mid1 = (uint8_t)((base >> 16) & 0xFF);
    desc->flags1 = 0x89; // P=1, DPL=0, type=1001b
    desc->flags2 = (uint8_t)((limit >> 16) & 0x0F);
    desc->base_mid2 = (uint8_t)((base >> 24) & 0xFF);
    desc->base_high = (uint32_t)((base >> 32) & 0xFFFFFFFF);
    desc->reserved = 0;
}

// Mira Kernel Load GDT
// Load GDT via LGDT and do a far‐jump to reload CS=0x08 (kernel code).
// Then clear DS/ES/SS/FS/GS so they all become 0x00 until we reload DS=0x10.
static inline void mk_load_gdt(const mk_gdt_ptr_t *gdtr) {
    __asm__ volatile (
        "lgdt (%0)\n\t"
        // Clear all data segments
        "xor %%rax, %%rax\n\t"
        "mov %%rax, %%ds\n\t"
        "mov %%rax, %%es\n\t"
        "mov %%rax, %%fs\n\t"
        "mov %%rax, %%gs\n\t"
        "mov %%rax, %%ss\n\t"
        // Far-jump to reload CS = 0x08
        "pushq $0x08\n\t"
        "lea 1f(%%rip), %%rax\n\t"
        "pushq %%rax\n\t"
        "lretq\n\t"
        "1:\n\t"
        :
        : "r"(gdtr)
        : "rax", "memory"
    );
}

// Mira Kernel Load TSS
static inline void mk_load_tss(uint16_t sel) {
    __asm__ volatile("ltr %0" : : "r"(sel) : "memory");
}

// Mira Kernel GDT Initialization
void mk_gdt_init(void) {
    // 1) Zero out GDT and TSS
    mk_memset(mk_gdt, 0, sizeof(mk_gdt));
    mk_memset(&mk_tss, 0, sizeof(mk_tss));

    // 2) Null descriptor at index 0 (already zeroed)

    // 3) Index 1 = Kernel code (DPL=0)
    mk_gdt_set_seg(&mk_gdt[MK_KERNEL_CODE_INDEX], 0, 1);

    // 4) Index 2 = Kernel data (DPL=0)
    mk_gdt_set_seg(&mk_gdt[MK_KERNEL_DATA_INDEX], 0, 0);

    // 5) Index 3 = User   code (DPL=3)
    mk_gdt_set_seg(&mk_gdt[MK_USER_CODE_INDEX], 3, 1);

    // 6) Index 4 = User   data (DPL=3)
    mk_gdt_set_seg(&mk_gdt[MK_USER_DATA_INDEX], 3, 0);

    // 7) Build TSS:
    // We reserve a real 4 KiB “kernel_stack” in mk_stack0[] and point TSS.rsp0 at its top.
    uintptr_t tss_base = (uintptr_t)&mk_tss;
    uint32_t tss_limit = (uint32_t)sizeof(mk_tss) - 1;

    // Set RSP0 to the TOP of our 4K buffer. Now ring 3 → ring 0 int 0x80 will push
    // onto that valid stack instead of RSP=0.
    mk_tss.rsp0 = (uint64_t)((uintptr_t)&mk_stack0[sizeof(mk_stack0)]);

    // 8) Index 5 & 6 = 16-byte TSS descriptor
    mk_gdt_set_tss((mk_tss_desc_t *)&mk_gdt[MK_TSS_INDEX], tss_base, tss_limit);

    // 9) Build GDTR and load the GDT
    mk_gdt_descriptor.limit = (uint16_t)(sizeof(mk_gdt) - 1);
    mk_gdt_descriptor.base = (uint64_t)(uintptr_t)&mk_gdt;
    mk_load_gdt(&mk_gdt_descriptor);

    // 10) Reload DS/ES/SS = 0x10 (kernel data selector)
    __asm__ volatile(
        "mov $0x10, %%ax\n\t"
        "mov %%ax, %%ds\n\t"
        "mov %%ax, %%es\n\t"
        "mov %%ax, %%fs\n\t"
        "mov %%ax, %%gs\n\t"
        "mov %%ax, %%ss\n\t"
        : : : "rax"
    );

    // 11) Load TSS into TR = selector 0x28
    mk_load_tss(MK_TSS_SELECTOR);
}
