#include "../include/tasks.h" 
mk_task ___mk_tasks[32]; // Array of tasks

int mk_create_task(uintptr_t base, uint32_t *stack) {
    int i;
    for (i = 0; i < 32; i++) {
        if (___mk_tasks[i].id == 0) {
            ___mk_tasks[i].id = i + 1;
            ___mk_tasks[i].base = base;
            ___mk_tasks[i].stack = stack;
            return i + 1;
        }
    }
    return 0;
}