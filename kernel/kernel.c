#include "inc/idt.h"
#include "inc/keyboard.h"
#include "inc/gdt.h"

extern int ms_entry();

int mk_entry() {
    mk_util_clear_screen();

    // Initialize the kernel components
    mk_gdt_init();
    mk_idt_init();
    mk_keyboard_init();
    
    // Create a task for the Mira Shell
    // Without a file system, we use a function pointer directly
    mk_task* task = mk_create_task_from_function(ms_entry, "Mira Shell");
    mk_execute_task(task);

    // Initialize the PIT
    // This is separated from the other initializations
    // because it enables interrupts and starts the scheduler
    mk_pit_init();

    /* mk_pit_init enables interrupts, so execution will not return here */
    /* Mira is now running :) */
    
    return 0;
}