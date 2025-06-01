#include "../inc/scheduler.h"
#include "../inc/util.h"
#include "../inc/gdt.h"

static int mk_scheduler_current_task = -1;
static mk_syscall_registers mk_scheduler_task_contexts[MK_TASKS_MAX]; // Array to hold task contexts

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
    // User mode tasks need a specific stack setup
    mk_task** all_tasks = mk_get_tasks();
    mk_task* next_task = all_tasks[next];
    if (new_rsp == 0) {
        if (next_task->mode == MK_TASKS_USER_MODE) {
            uintptr_t kernel_top = next_task->stack_ptr;
            uint64_t *stack = (uint64_t*)kernel_top;

            // (a) push SS
            *--stack = MK_USER_DATA_SELECTOR;

            // (b) push RSP (user stack pointer)
            *--stack = (uint64_t)next_task->user_stack_ptr;

            // (c) push RFLAGS (0x202 sets IF=1)
            *--stack = 0x202;

            // (d) push CS
            *--stack = MK_USER_CODE_SELECTOR;

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

    // Install the next task’s kernel stack in the TSS 
    // This is necessary for ring transitions
    extern mk_tss_t mk_tss;
    mk_tss.rsp0 = (uint64_t)next_task->stack_ptr;

    // 5. Load the task’s RSP and jump/iretq
    if (next_task->mode == MK_TASKS_KERNEL_MODE) {
        __asm__ volatile (
            "movq %0, %%rsp\n\t"
            "jmp  *%1\n\t"
            :
            : "r"(new_rsp), "r"(next_task->base)
            : "memory"
        );
    } else {
        __asm__ volatile (
            "movq %0, %%rsp\n\t"
            "iretq\n\t"
            :
            : "r"(new_rsp)
            : "memory"
        );
    }
}
