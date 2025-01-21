// kernel.c

#include <stdint.h>     // For fixed-width integer types like uint16_t, etc.
#include "include/idt.h"  // Assuming you use mk_idt_init(), etc.


// Simple PIT initialization, sets divisor to achieve desired frequency
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static void init_pit(uint32_t frequency) {
    //
    // 1) Remap the PIC so that IRQ0 → IDT 0x20 … IRQ7 → IDT 0x27, etc.
    //
    outb(0x20, 0x11);  // Start init on master PIC
    outb(0xA0, 0x11);  // Start init on slave PIC
    outb(0x21, 0x20);  // Master PIC vector offset = 0x20
    outb(0xA1, 0x28);  // Slave PIC vector offset = 0x28
    outb(0x21, 0x04);  // Tell master PIC there is a slave at IRQ2
    outb(0xA1, 0x02);  // Tell slave PIC its cascade identity
    outb(0x21, 0x01);  // 8086/88 (MCS-80/85) mode
    outb(0xA1, 0x01);  // 8086/88 (MCS-80/85) mode

    //
    // 2) Program PIT channel 0 to desired frequency
    //
    if (frequency == 0) {
        frequency = 1;  // Prevent divide-by-zero
    }
    uint16_t divisor = (uint16_t)(1193180 / frequency);

    // Mode 3 (square wave), access mode: lobyte/hibyte, channel 0
    outb(0x43, 0x36);

    // Send low byte, then high byte to channel 0
    outb(0x40, (uint8_t)(divisor & 0xFF));
    outb(0x40, (uint8_t)((divisor >> 8) & 0xFF));

    //
    // 3) Unmask IRQ0 on the master PIC
    //
    uint8_t mask = inb(0x21);
    mask &= ~(1 << 0); // Clear bit 0 to allow PIT interrupts
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
    init_pit(1000);

    __asm__ volatile("sti");

    // Spin or continue your OS kernel flow
    for (;;) {
        // Do nothing or schedule tasks, etc.
    }

    return 0;
}