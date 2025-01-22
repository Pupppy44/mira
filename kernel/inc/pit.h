#ifndef PIT_H
#define PIT_H

#include <stdint.h>

#define MK_PIT_FREQUENCY 1000 // 1000 Hz (1ms)

// Function to initialize the PIT
void mk_pit_init();

// Function to handle PIT interrupts
void mk_pit_handler();

// Function to get the PIT tick count
uint64_t mk_pit_get_tick_count();

#endif