#include "../include/idt.h"
// for rand()

// custom rand function
int rand() {
    static unsigned long x = 123456789, y = 362436069, z = 521288629, w = 88675123;
    unsigned long t;
    t = x ^ (x << 11);
    x = y; y = z; z = w;
    return w = w ^ (w >> 19) ^ (t ^ (t >> 8));
}

#define IDT_ENTRIES 256
mk_idt_entry idt[IDT_ENTRIES];
mk_idt_ptr idt_ptr;

// Function to set an IDT entry
void set_idt_entry(mk_idt_entry *entry, uintptr_t handler, uint16_t segment_selector, uint8_t type_attributes) {
    entry->offset_low = handler & 0xFFFF;
    entry->segment_selector = segment_selector;
    entry->ist = 0; // Use IST 0
    entry->reserved0 = 0;
    entry->type_attributes = type_attributes;
    entry->offset_middle = (handler >> 16) & 0xFFFF;
    entry->offset_high = (handler >> 32) & 0xFFFFFFFF;
    entry->reserved1 = 0;
}

// Assembly function to load the IDT
void lidt(mk_idt_ptr *idt_ptr) {
    __asm__ volatile (
        "lidt %0"
        : : "m" (*idt_ptr)
    );
}

// Default handler for unhandled interrupts (for now)
void default_interrupt_handler() {
    char randomLetter = 'A' + (char)(rand() % 26);
    *(uint8_t*)0xb8000 = randomLetter;
    *(uint8_t*)0xb8001 = 0x04;
    while (1);
}

void syscall_interrupt() {
}

// Initialize the IDT
void idt_init() {
    idt_ptr.limit = sizeof(idt) - 1;
    idt_ptr.base = (uintptr_t)&idt;

    // Set all IDT entries to a default handler
    for (int i = 0; i < IDT_ENTRIES; i++) {
        set_idt_entry(&idt[i], (uintptr_t)default_interrupt_handler, KERNEL_CODE_SELECTOR, 0x8E); // Ring 0
    }

    // Set the IDT entry for the syscall interrupt (0x80)
    set_idt_entry(&idt[0x80], (uintptr_t)syscall_handler, KERNEL_CODE_SELECTOR, 0xEE); // Ring 3

    // Load the IDT
    lidt(&idt_ptr);
}