#include "../inc/tasks.h" 

static mk_task* mk_tasks[32] = {0};
static int mk_tasks_count = 0;

mk_task* mk_create_task(unsigned char* shellcode, size_t shellcode_size, const char* name) {
    mk_task* new_task = (mk_task*)mk_malloc(sizeof(mk_task));
    
    if (!new_task) {
        return NULL; // Allocation failed
    }

    new_task->id = mk_tasks_count++;
    new_task->name = name;
    new_task->base = (uintptr_t)mk_malloc(shellcode_size);
    new_task->stack = (uintptr_t)mk_malloc(4096); // 4KB stack
    new_task->stack_ptr = new_task->stack + 4096; // Stack start (grows down)
    new_task->status = 0; // Not running
    
    if (!new_task->base || !new_task->stack) {
        return NULL; // Allocation failed
    }

    // Copy the shellcode to the allocated memory
    unsigned char* dest = (unsigned char*)new_task->base;
    for (size_t i = 0; i < shellcode_size; ++i) {
        dest[i] = shellcode[i];
    }

    mk_tasks[new_task->id] = new_task;
    
    return new_task;
}

// New function for creating tasks from C functions
mk_task* mk_create_task_from_function(int (*entry_point)(void), const char* name) {
    mk_task* new_task = (mk_task*)mk_malloc(sizeof(mk_task));
    if (!new_task) {
        return NULL; // Allocation failed
    }

    new_task->id = mk_tasks_count++;
    new_task->name = name;
    new_task->base = (uintptr_t)entry_point; // Directly use the function's address
    new_task->stack = (uintptr_t)mk_malloc(4096); // 4KB stack
    new_task->stack_ptr = new_task->stack + 4096; // Stack start (grows down)
    new_task->status = 0; // Not running

    if (!new_task->base || !new_task->stack) {
        return NULL; // Allocation failed
    }
    
    mk_tasks[new_task->id] = new_task;
    return new_task;
}

void mk_execute_task(mk_task* task) {
    if (task && task->base) {
        // By setting the status as running, the PIT scheduler will pick it up
        task->status = 1;
    }
}

mk_task** mk_get_tasks() {
    return mk_tasks;
}

int mk_get_task_count() {
    return mk_tasks_count;
}