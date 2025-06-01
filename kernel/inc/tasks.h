#ifndef TASKS_H
#define TASKS_H

#include "mem.h"

#define MK_TASKS_MAX 32 // Maximum number of tasks
#define MK_TASKS_NOT_RUNNING 0
#define MK_TASKS_RUNNING 1
#define MK_TASKS_KERNEL_MODE 0
#define MK_TASKS_USER_MODE 1

// Structure for all Mira tasks
typedef struct _mk_task {
    int id; // Task ID
    const char* name; // Name of the task
    uintptr_t base; // Base address of the task's code
    uintptr_t stack; // Base address of the task's stack
    uintptr_t stack_ptr; // Pointer to the top of the task's stack
    uintptr_t user_stack_base; // Base address for user stack
    uintptr_t user_stack_ptr; // Pointer to the top of the user stack
    int status; // 0 = Not Running, 1 = Running
    int mode; // 0 = Kernel, 1 = User
} mk_task;

mk_task* mk_create_task(unsigned char* shellcode, size_t shellcode_size, const char* name);

mk_task* mk_create_task_from_function(int (*entry_point)(void), const char* name);

void mk_execute_task(mk_task* task);

mk_task** mk_get_tasks();

int mk_get_task_count();

#endif