#include "../include/idt.h"

#define MK_IDT_ENTRIES 256
mk_idt_entry idt[MK_IDT_ENTRIES];
mk_idt_ptr idt_ptr;

// Mira Kernel IDT Set Entry
void mk_idt_set_entry(mk_idt_entry *entry, uintptr_t handler, uint16_t segment_selector, uint8_t type_attributes) {
    entry->offset_low = handler & 0xFFFF;
    entry->segment_selector = segment_selector;
    entry->ist = 0; // Use IST 0
    entry->reserved0 = 0;
    entry->type_attributes = type_attributes;
    entry->offset_middle = (handler >> 16) & 0xFFFF;
    entry->offset_high = (handler >> 32) & 0xFFFFFFFF;
    entry->reserved1 = 0;
}

// Mira Kernel IDT Loader
void mk_idt_load(mk_idt_ptr *idt_ptr) {
    __asm__ volatile (
        "lidt %0"
        : : "m" (*idt_ptr)
    );
}

// Mira Kernel IDT Handler
void mk_idt_handler() {
    while (1);
}


// Initialize the IDT
void mk_idt_init() {
    idt_ptr.limit = sizeof(idt) - 1;
    idt_ptr.base = (uintptr_t)&idt;

    // Set all IDT entries to a default handler
    for (int i = 0; i < MK_IDT_ENTRIES; i++) {
        mk_idt_set_entry(&idt[i], (uintptr_t)mk_idt_handler, MK_CODE_SELECTOR, 0x8E); // Ring 0
    }

    // Set the IDT entry for the syscall interrupt (0x80)
    mk_idt_set_entry(&idt[0x80], (uintptr_t)mk_syscall_handler, MK_CODE_SELECTOR, 0xEE); // Ring 3

    // Load the IDT
    mk_idt_load(&idt_ptr);
}