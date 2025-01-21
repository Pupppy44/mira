#include "../include/idt.h"
#include "../include/tasks.h"

#define MK_IDT_ENTRIES 256
mk_idt_entry idt[MK_IDT_ENTRIES];
mk_idt_ptr idt_ptr;

char randChar() {
    static unsigned int seed = 0;
    seed = seed * 1103515245 + 12345;
    return (seed / 65536) % 150 + 50; // Between 50 and 200
}

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

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
void mk_idt_handler(void) {
    // Reenable interrupts asap so other interrupts can be handled
    outb(0x20, 0x20);
    __asm__ volatile ("sti");

    // Write random character to video memory
    *(unsigned char*)0xb8000 = randChar();
    *(unsigned char*)0xb8001 = 0x03;

    // Get the value of rax (assuming we want the lower 8 bits as a 2-digit number)
    uint64_t rax;
    __asm__ volatile ("mov %%rax, %0" : "=r"(rax));
uint8_t value = (uint8_t)(rax & 0xFF);
    if (value != 555) {
            // Extract the lower byte of rax

    // Extract tens and ones digits
    uint8_t tens = value / 10;  // Tens place
    uint8_t ones = value % 10;  // Ones place

    // Convert to ASCII characters (digit '0' is 48 in ASCII)
    unsigned char tens_ascii = '0' + tens;
    unsigned char ones_ascii = '0' + ones;

    // Write tens digit to 0xb8012
    *(unsigned char*)0xb8012 = tens_ascii;
    *(unsigned char*)0xb8013 = 0x03;  // Set attribute byte (color code)

    // Write ones digit to 0xb8014
    *(unsigned char*)0xb8014 = ones_ascii;
    *(unsigned char*)0xb8015 = 0x03;  // Set attribute byte (color code)
    }

    // Pop the error code from the stack and return to the caller
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

// Mira Kernel IDT PIT Handler
void mk_idt_pit_handler(void) {
    // Reenable interrupts asap so other interrupts can be handled
    outb(0x20, 0x20);
    __asm__ volatile ("sti");

    // Get the interrupt number
    uint64_t rax;
    __asm__ volatile ("mov %%rax, %0" : "=r"(rax));

    *(unsigned char*)0xb8006 = randChar();
    *(unsigned char*)0xb8007 = 0x05;

    // Pop the error code from the stack and return to the caller
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

// Initialize the IDT
void mk_idt_init() {
    idt_ptr.limit = sizeof(mk_idt_entry) * MK_IDT_ENTRIES - 1;
    idt_ptr.base = (uint64_t)&idt;

    // Set all IDT entries to a default handler
    for (int i = 0; i < MK_IDT_ENTRIES; i++) {
        mk_idt_set_entry(&idt[i], (uintptr_t)mk_idt_handler, MK_CODE_SELECTOR, 0x8E); // Ring 0
    }

    // Set the IDT entry for the syscall interrupt (0x80)
    //mk_idt_set_entry(&idt[0x80], (uintptr_t)mk_syscall_handler, MK_CODE_SELECTOR, 0xEE); // Ring 3

    // Set the IDT entry for the PIT timer interrupt (0x32)
    mk_idt_set_entry(&idt[0x20], (uintptr_t)mk_idt_pit_handler, MK_CODE_SELECTOR, 0x8E); // Ring 0

    // Load the IDT
    mk_idt_load(&idt_ptr);
}