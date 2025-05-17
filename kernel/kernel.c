#include "inc/idt.h"

extern int ms_entry();

// Mira Kernel Entry
int mk_entry() {    
    mk_util_clear_screen();
 
    mk_util_print("Mira Kernel Booting...");
    mk_create_task_from_function(ms_entry, "Mira User Mode Shell");

    mk_idt_init();
    mk_pit_init();

    while (1);

    return 0;
}