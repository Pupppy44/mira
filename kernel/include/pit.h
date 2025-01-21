#ifndef PIT_H
#define PIT_H

#include <stdint.h>

// Function prototypes
void init_pit(uint32_t frequency);
void unmask_irq0_on_pic();
void enable_interrupts();

// Constants
#define PIT_BASE_FREQUENCY 1193180 // Base frequency of the PIT

// Macros for common frequencies
#define PIT_FREQUENCY_100HZ 100
#define PIT_FREQUENCY_1000HZ 1000

#endif // PIT_H
