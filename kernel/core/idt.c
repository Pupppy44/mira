#include "../include/idt.h"

#define MK_IDT_ENTRIES 256
mk_idt_entry_t mk_idt[MK_IDT_ENTRIES];
mk_idt_ptr_t mk_idt_ptr;

// Mira Kernel IDT Set Entry
void mk_idt_set_entry(mk_idt_entry_t *entry, uintptr_t handler, uint16_t segment_selector, uint8_t type_attributes) {
    entry->offset_low = handler & 0xFFFF;
    entry->segment_selector = segment_selector;
    entry->ist = 0; // Use IST 0, the default error stack handler
    entry->reserved0 = 0;
    entry->type_attributes = type_attributes;
    entry->offset_middle = (handler >> 16) & 0xFFFF;
    entry->offset_high = (handler >> 32) & 0xFFFFFFFF;
    entry->reserved1 = 0;
}

// Mira Kernel IDT Loader
void mk_idt_load(mk_idt_ptr_t *idt_ptr) {
    __asm__ volatile (
        "lidt %0"
        : : "m" (*idt_ptr)
    );
}

// Mira IDT Pre Handler - Acknowledges the interrupt and reenables interrupts
// Without doing this early, the PIT and other interrupts will be missed or delayed
void mk_idt_pre_handler() {
    mk_util_outb(0x20, 0x20);
    __asm__ volatile ("sti");
}

// Mira IDT Post Handler - Restores registers and returns to the caller
// Without this final step, the system will subsequently crash
void mk_idt_post_handler() {
    __asm__ volatile (
        "pop %r15\n"
        "pop %r14\n"
        "pop %r13\n"
        "pop %r12\n"
        "pop %r11\n"
        "pop %r10\n"
        "pop %r9\n"
        "pop %r8\n"
        "pop %rbp\n"
        "pop %rdi\n"
        "pop %rsi\n"
        "pop %rdx\n"
        "pop %rcx\n"
        "pop %rbx\n"
        "pop %rax\n"
        "iretq"
    );
}

// Mira Kernel IDT Default Handler
void mk_idt_default_handler(void) {
    mk_idt_pre_handler();
    mk_idt_post_handler();
}

// Initialize the IDT
void mk_idt_init() {
    mk_idt_ptr.limit = sizeof(mk_idt_entry_t) * MK_IDT_ENTRIES - 1;
    mk_idt_ptr.base = (uint64_t)&mk_idt;

    // Set all IDT entries to a default handler
    for (int i = 0; i < MK_IDT_ENTRIES; i++) {
        mk_idt_set_entry(&mk_idt[i], (uintptr_t)mk_idt_default_handler, MK_CODE_SELECTOR, 0x8E); // Ring 0
    }

    // Set the IDT entry for the syscall interrupt (0x80)
    mk_idt_set_entry(&mk_idt[0x80], (uintptr_t)mk_syscall_handler, MK_CODE_SELECTOR, 0xEE); // Ring 3

    // Set the IDT entry for the PIT timer interrupt (0x20)
    mk_idt_set_entry(&mk_idt[0x20], (uintptr_t)mk_pit_handler, MK_CODE_SELECTOR, 0x8E); // Ring 0

    // Load the IDT
    mk_idt_load(&mk_idt_ptr);
}