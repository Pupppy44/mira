// pit.c
#include <stdint.h>

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

void init_pit(uint32_t frequency) {
    uint32_t divisor = 1193180 / frequency;

    // Command byte: Channel 0, access mode: lobyte/hibyte, operating mode: 3 (square wave)
    outb(0x43, 0x36);

    // Write frequency divisor
    outb(0x40, (uint8_t)(divisor & 0xFF));
    outb(0x40, (uint8_t)((divisor >> 8) & 0xFF));
}

void unmask_irq0_on_pic() {
    uint8_t mask = 0xFF & ~(1 << 0); // Unmask IRQ0
    outb(0x21, mask);
}

void enable_interrupts() {
    __asm__ volatile ("sti");
}