#ifndef SCHEUDLER_H
#define SCHEUDLER_H

#include <stdint.h>
#include "syscalls.h"
#include "tasks.h"

#define MK_USER_CODE_SELECTOR 0x1B
#define MK_USER_DATA_SELECTOR 0x23

// Function to step the task scheduler
void mk_scheduler_step();

#endif