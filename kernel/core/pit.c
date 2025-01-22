#include "../include/pit.h"
#include "../include/idt.h"
#include "../include/scheduler.h"

static uint64_t mk_pit_ticks = 0;

// Mira Kernel PIT Initialization
void mk_pit_init() {
    // Remaps IRQ0 to the PIT
    mk_util_outb(0x20, 0x11);  // Start init on master PIC
    mk_util_outb(0xA0, 0x11);  // Start init on slave PIC
    mk_util_outb(0x21, 0x20);  // Master PIC vector offset = 0x20
    mk_util_outb(0xA1, 0x28);  // Slave PIC vector offset = 0x28
    mk_util_outb(0x21, 0x04);  // Tell master PIC there is a slave at IRQ2
    mk_util_outb(0xA1, 0x02);  // Tell slave PIC its cascade identity
    mk_util_outb(0x21, 0x01);  // 8086/88 (MCS-80/85) mode
    mk_util_outb(0xA1, 0x01);  // 8086/88 (MCS-80/85) mode

    // Set the PIT to the desired frequency
    uint16_t divisor = (uint16_t)(1193180 / MK_PIT_FREQUENCY);

    mk_util_outb(0x43, 0x36); // Square Wave Mode for the PIT 

    // Send the divisor to the PIT (based on the frequency)
    mk_util_outb(0x40, (uint8_t)(divisor & 0xFF));
    mk_util_outb(0x40, (uint8_t)((divisor >> 8) & 0xFF));

    // Unmask IRQ0 on the PIC to allow PIT interrupts
    uint8_t mask = mk_util_inb(0x21);
    mask &= ~(1 << 0); // Clear bit 0 to allow PIT interrupts
    mk_util_outb(0x21, mask);

    // Enable interrupts, the PIT now runs
    __asm__ volatile ("sti");
}

// Mira Kernel PIT Handler
void mk_pit_handler() {
    mk_idt_pre_handler();

    mk_pit_ticks++;

    mk_scheduler_step();

    mk_idt_post_handler();
}

// Mira Kernel Get PIT Tick Count
uint64_t mk_pit_get_tick_count() {
    return mk_pit_ticks;
}