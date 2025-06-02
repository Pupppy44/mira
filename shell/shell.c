#include <stdint.h>

// TODO: Support multiple attributes for text color
void mira_print(const char* output, uint8_t attribute) {
    __asm__ volatile (
        "mov $1, %%rax\n\t"
        "mov %0, %%rdi\n\t"
        "mov %1, %%rsi\n\t"
        "int $0x80"
        : : "r"(output), "r"((uint64_t)attribute) : "rax","rdi","rsi"
    );
}

void ms_entry() {
    mira_print("Welcome to the Mira Shell!\n", 0x07);
    mira_print("Type 'help' for a list of commands.\n", 0x07);

    for (;;) {
        asm volatile (
            "hlt"  // Halt the CPU until the next interrupt
        );
    }
}
