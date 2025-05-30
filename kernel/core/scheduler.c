#include "../inc/scheduler.h"
#include "../inc/util.h"
#include "../inc/keyboard.h"

#define KERNEL_MODE        0
#define USER_MODE          1
#define USER_CODE_SELECTOR 0x1B
#define USER_DATA_SELECTOR 0x23

static int mk_scheduler_current_task = -1;
static mk_syscall_registers mk_scheduler_task_contexts[32]; // Assuming a max of 32 tasks

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
    // 1. Save registers of the current task (only RSP needed for now)
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

    // 3. Restore next task's RSP (kernel stack) and prepare for switch
    uintptr_t new_rsp = mk_scheduler_task_contexts[next].rsp;

    // If the new task is just starting, initialize its stack/frame
    mk_task** all_tasks = mk_get_tasks();
    mk_task* next_task = all_tasks[next];
    
    if (new_rsp == 0) {
        if (next_task->mode == USER_MODE) {
            uintptr_t kernel_top = next_task->stack_ptr;
            uint64_t *stack = (uint64_t*)kernel_top;

            // (a) push SS
            *--stack = USER_DATA_SELECTOR;

            // (b) push RSP (user stack pointer)
            *--stack = (uint64_t)next_task->user_stack_ptr;

            // (c) push RFLAGS (0x202 sets IF=1)
            *--stack = 0x202;

            // (d) push CS
            *--stack = USER_CODE_SELECTOR;

            // (e) push RIP
            *--stack = (uint64_t)next_task->base;

            mk_scheduler_task_contexts[next].rsp = (uintptr_t)stack;
            new_rsp = (uintptr_t)stack;
        } else {
            // First time running a kernel-mode task: initialize its stack/frame
            new_rsp = (uintptr_t)next_task->stack_ptr;
            mk_scheduler_task_contexts[next].rsp = new_rsp;
        }
    }

    // Update current task index for the next interrupt
    mk_scheduler_current_task = next;

    // 4. Switch to the next task
    if (next_task->mode == KERNEL_MODE) {
        __asm__ volatile (
            "movq %0, %%rsp\n\t"   // Load next task's kernel RSP
            "jmp *%1\n\t"          // Jump to the kernel task's entry point
            :
            : "r"(new_rsp), "r"(next_task->base)
            : "memory"
        );
    } else {
        __asm__ volatile (
            "movq %0, %%rsp\n\t"   // Load next task's kernel RSP
            "iretq\n\t"            // Switch to user mode
            :
            : "r"(new_rsp)
            : "memory"
        );
    }
}
