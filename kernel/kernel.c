#include "include/idt.h"
#include "include/tasks.h"
#include <stdint.h>

// Round-robin scheduling variables
int current_process = 0;
register_t saved_context; // For saving context during interrupts

extern void user_regs_switch(register_t* regs);

// Round-robin scheduler function
void scheduler() {
    mk_task** task_list = mk_get_tasks();
    int num_tasks = mk_get_task_count();

    if (num_tasks == 0) {
        // No tasks to schedule, just loop
        while (1);
    }

    while (1) {
        // Get the current and next task
        mk_task *current_task = task_list[current_process];
        int next_process = (current_process + 1) % num_tasks;
        mk_task *next_task = task_list[next_process];

        // Save current task's registers
        // (Assuming 'saved_context' is used when the scheduler is called via an interrupt)
        if (current_task != NULL) {
            saved_context.eax = current_task->regs.eax;
            saved_context.ebx = current_task->regs.ebx;
            saved_context.ecx = current_task->regs.ecx;
            saved_context.edx = current_task->regs.edx;
            saved_context.esi = current_task->regs.esi;
            saved_context.edi = current_task->regs.edi;
            saved_context.ebp = current_task->regs.ebp;
            saved_context.esp = current_task->regs.esp;
            saved_context.useresp = current_task->regs.useresp;
            saved_context.eflags = current_task->regs.eflags;
            saved_context.eip = current_task->regs.eip;
            saved_context.cr3 = current_task->regs.cr3;
        }

        // Switch to the next process by loading its registers
        user_regs_switch(&next_task->regs);

                *(unsigned char*)0xb8000 = 'H';
        *(unsigned char*)0xb8001 = 0x05;

        // The assembly function 'user_regs_switch' will handle the actual switch.
        // Execution will resume in the next task after the iret.

        // Update the current process index
        current_process = next_process;
    }
}

// Mira Kernel Entry Point
int mk_entry() {
    mk_idt_init();

    // Example shellcode - we'll leave this part as is for now
    unsigned char shellcode[] = { 0xC6, 0x04, 0x25, 0x00, 0x80, 0x0B, 0x00, 0x4D, 0xC6, 0x04, 0x25, 0x01, 0x80, 0x0B, 0x00, 0x03 };

    // Create the tasks using mk_create_task (which sets stack and base)
    mk_task *user_task1 = mk_create_task(shellcode, sizeof(shellcode));

    user_task1->regs.eip = (uintptr_t)user_task1->base; // Set the instruction pointer to the base address
    user_task1->regs.esp = (uintptr_t)user_task1->stack; // Set the stack pointer to the top of the stack
    user_task1->regs.useresp = user_task1->regs.esp; // Set user stack pointer
    user_task1->regs.eflags = 0x202; // Enable interrupts (example)

    // Start the round-robin scheduler
    scheduler();

    return 0;
}