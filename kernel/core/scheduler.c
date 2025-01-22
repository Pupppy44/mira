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
        return; 
    }

    // 3. Restore next task's registers (rsp) and jump there
    uintptr_t new_rsp = mk_scheduler_task_contexts[next].rsp;

    // If the new task is just starting, initialize it
    mk_task** all_tasks = mk_get_tasks();
    mk_task* next_task = all_tasks[next];
    if (new_rsp == 0) {
        // The task has not run yet. Set it to the top of its allocated stack.
        new_rsp = next_task->stack_ptr;
        mk_scheduler_task_contexts[next].rsp = new_rsp;
    }

    // Switch the current task to the next task for the next PIT interrupt
    mk_scheduler_current_task = next;

    // 4. Switch to the next task
    // Use IRET-based approach in real code or ring transitions. Here, a simplified ring-0 example:
    __asm__ volatile (
        "movq %0, %%rsp\n\t"   // Load next task RSP
        // The simplest approach is a "jmp" to the code, but if the code has jmp $,
        // we rely on the next timer interrupt to switch again.
        "jmp *%1\n\t"
        :
        : "r"(new_rsp), "r"(next_task->base)
        : "memory"
    );
}