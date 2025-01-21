#include "include/idt.h"

// Mira Kernel Entry
int mk_entry() {
    mk_util_clear_screen();

    mk_idt_init();
    mk_pit_init();

    int index = 0;

    while (1) {
    for (int i = 0; i < 1000000; i++) {
    }
        mk_util_print("tick ");
    }

    return 0;
}