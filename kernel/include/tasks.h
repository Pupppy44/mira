#ifndef TASKS_H
#define TASKS_H

#include <stdint.h>

// Structure for all Mira tasks
typedef struct _mk_task {
    int id;
    uintptr_t base; // Base address of the task's code in memory
    uint32_t *stack; // Pointer to the task's stack
} mk_task;

int mk_create_task(uintptr_t base, uint32_t *stack);

#endif