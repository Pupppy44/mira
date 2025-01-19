#include "../include/tasks.h" 

static mk_task* mk_tasks[32] = {0};
static int mk_tasks_count = 0;

mk_task* mk_create_task(unsigned char* shellcode, size_t shellcode_size) {
    mk_task* new_task = (mk_task*)mk_malloc(sizeof(mk_task));
    if (!new_task) {
        return NULL; // Allocation failed
    }
                                                                  
    new_task->id = mk_tasks_count++;
    new_task->base = (uintptr_t)mk_malloc(shellcode_size);
    if (!new_task->base) {
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

void mk_execute_task(mk_task* task) {
    if (task && task->base) {
        void (*func)() = (void (*)())task->base;
        func();
    }
}