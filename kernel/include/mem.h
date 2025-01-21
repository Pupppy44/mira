#ifndef MEM_H
#define MEM_H

#include <stddef.h>
#include <stdint.h>

#define MK_HEAP_START 0x20000 // Kernel starts at 0x8200, so this should be fine
#define MK_HEAP_SIZE (1024 * 1024) // 1MB heap

void* mk_malloc(size_t size);

#endif