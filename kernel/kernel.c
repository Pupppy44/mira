// kernel.c

#include <stdint.h>     // For fixed-width integer types like uint16_t, etc.
#include "include/idt.h"  // Assuming you use mk_idt_init(), etc.


// Inline assembly for input from an I/O port (no standard library)
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// Inline assembly for output to an I/O port (no standard library)
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

// Simple PIT initialization, sets divisor to achieve desired frequency
static void init_pit(uint32_t frequency) {
    // The PIT input clock is ~1.193182 MHz
    uint16_t divisor = (uint16_t)(1193180 / frequency);

    // 0x36 sets channel 0, low/high byte, rate generator mode
    outb(0x43, 0x36);

    // Send low byte, then high byte
    outb(0x40, (uint8_t)(divisor & 0xFF));
    outb(0x40, (uint8_t)((divisor >> 8) & 0xFF));

    // Unmask IRQ0 on Master PIC
    // Read the current mask
    uint8_t mask = inb(0x21);
    // Clear the bit for IRQ0
    mask &= ~(1 << 0);
    // Write back to unmask
    outb(0x21, mask);
}

//
// mk_entry: strict entry point of the kernel
//  1) Initialize IDT
//  2) Initialize the PIT
//  3) Enable interrupts (sti)
//  4) Enter an infinite loop or proceed with your OS flow
//
int mk_entry() {
    // Initialize the IDT (assumes mk_idt_init() is in IDT.c)
    mk_idt_init();

    // Initialize the PIT to (for example) 100 Hz
    init_pit(100);

    __asm__ volatile("sti");

    // Spin or continue your OS kernel flow
    for (;;) {
        // Do nothing or schedule tasks, etc.
    }

    return 0;
}