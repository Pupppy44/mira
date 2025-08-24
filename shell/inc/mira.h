#ifndef MIRA_H
#define MIRA_H

#include <stdint.h>

// User-space definition of the mouse state structure.
// ! Must match the kernel's mk_mouse_state_t.
typedef struct {
    int32_t x;
    int32_t y;
    uint8_t left_button;
    uint8_t right_button;
    uint8_t middle_button;
} mira_mouse_state_t;

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

// Mira Get Mouse State Function
// Takes a pointer to a mira_mouse_state_t struct to be filled by the kernel.
static inline void mira_get_mouse_state(mira_mouse_state_t* state_ptr) {
    __asm__ volatile (
        "mov $3, %%rax\n\t"  // Syscall number for getting mouse state
        "mov %0, %%rdi\n\t"  // Pass the pointer as the first argument
        "int $0x80"
        : : "r"(state_ptr) : "rax", "rdi"
    );
}

#endif