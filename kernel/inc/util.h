#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
#include "pit.h"

static inline uint8_t mk_util_inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void mk_util_outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline void mk_util_sleep(uint64_t ms) {
    uint64_t start = mk_pit_get_tick_count();
    while (mk_pit_get_tick_count() < start + ms);
}

#endif