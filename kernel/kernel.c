// Mira Kernel Keyboard Testing //
// 5/17/2025 //

#include "inc/idt.h"
#include "inc/keyboard.h"

extern int ms_entry();

int mk_entry() {
    mk_util_clear_screen();
    mk_keyboard_init();

    while (1) {
        const char* key = mk_keyboard_get_key();
        if (key) {
            mk_util_print("Key pressed: ");
            mk_util_print(key);
            mk_util_print("\n");
        }
    }
    
    return 0;
}