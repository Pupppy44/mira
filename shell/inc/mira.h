#ifndef MIRA_H
#define MIRA_H

#include <stdint.h>

// Mira User Mode System Calls //

// Mira Print Function
// TODO: Support more attributes
void mira_print(const char* string, uint8_t attribute) {
    __asm__ volatile (
        "mov $1, %%rax\n\t"
        "mov %0, %%rdi\n\t"
        "mov %1, %%rsi\n\t"
        "int $0x80"
        : : "r"(string), "r"((uint64_t)attribute) : "rax","rdi","rsi"
    );
}

// Mira Get Keyboard Key Function
char mira_get_key(void) {
    uint64_t key = 0;

    __asm__ volatile (
        "mov $2, %%rax\n\t"
        "int $0x80\n\t"
        : "=a"(key) ::
          "rcx","rsi","rdi","rdx","r8","r9","r10","r11");
          
    return (char)key;
}

#endif