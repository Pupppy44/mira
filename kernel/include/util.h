#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

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

static inline void mk_util_print(const char* str) {
    static uint16_t cursor = 0;
    static uint16_t* video_memory = (uint16_t*)0xb8000;

    for (size_t i = 0; str[i] != '\0'; i++) {
        video_memory[cursor] = (video_memory[cursor] & 0xFF00) | str[i];
        cursor++;
    }
}

static inline void mk_util_clear_screen() {
    static uint16_t* video_memory = (uint16_t*)0xb8000;
    for (size_t i = 0; i < 80 * 25; i++) {
        video_memory[i] = (video_memory[i] & 0xFF00) | ' ';
    }
}

#endif