#include "../inc/scheduler.h"
#include "../inc/util.h"

static int mk_scheduler_current_task = -1;
static mk_syscall_registers mk_scheduler_task_contexts[];  

// Mira Kernel Scheduler Get Next Task
int mk_scheduler_get_next_task() {
    int task_count = mk_get_task_count();
    if (task_count == 0) {
        return -1;
    }

    // Round-robin scheduler where we just cycle through the tasks
    mk_scheduler_current_task = (mk_scheduler_current_task + 1) % task_count;

    return mk_scheduler_current_task;
}

// Mira Kernel Task Scheduler Step
void mk_scheduler_step() {
    // 1. Save registers of the current task
    uintptr_t saved_rsp;
    __asm__ volatile ("movq %%rsp, %0" : "=r" (saved_rsp));
    int old_task = mk_scheduler_current_task;
    if (old_task >= 0) {
        mk_scheduler_task_contexts[old_task].rsp = saved_rsp;
    }

    // 2. Get next task index
    int next = mk_scheduler_get_next_task();
    if (next < 0) {
        // No tasks to schedule
        return; 
    }

    // 3. Restore next task's registers (rsp)
    mk_task** all_tasks = mk_get_tasks();
    mk_task* next_task = all_tasks[next];
    uintptr_t new_rsp = mk_scheduler_task_contexts[next].rsp;
    
    // If the new task is just starting, initialize it
    if (new_rsp == 0) {
        new_rsp = next_task->stack_ptr;
        mk_scheduler_task_contexts[next].rsp = new_rsp;
    }

    // Switch the current task index
    mk_scheduler_current_task = next;

    // 4. Depending on mode, switch to kernel or user
    if (next_task->mode == 0) {
        // Kernel task (ring 0), just jump to its base
        __asm__ volatile (
            "movq %0, %%rsp\n\t"   // Load next task RSP
            "jmp *%1\n\t"         // Jump to task->base at ring 0
            :
            : "r"(new_rsp), "r"(next_task->base)
            : "memory"
        );
    } else {
        // User task (ring 3) - simplified example using iretq
        __asm__ volatile (
            "movq %0, %%rsp\n\t"        // Set up new RSP for the user task
            "pushq $0x23\n\t"           // User data segment selector
            "pushq %0\n\t"              // Push user stack pointer
            "pushfq\n\t"                // Push RFLAGS
            "pushq $0x1B\n\t"           // User code segment selector
            "pushq %1\n\t"              // Push instruction pointer (task->base)
            "iretq\n\t"                 // Return to ring 3
            :
            : "r"(new_rsp), "r"(next_task->base)
            : "memory"
        );
    }

    // If we reach here, something unexpected happened
    // because normally we would have switched contexts.
}