#ifndef MEM_H
#define MEM_H

#include <stddef.h>
#include <stdint.h>

#define MK_HEAP_START 0x80000 // Kernel starts at 0x7E00, so this should be fine
#define MK_HEAP_SIZE (1024 * 1024 * 4) // 4MB heap

void* mk_malloc(size_t size);

void mk_free(void* ptr);

void* mk_memset(void* ptr, int value, size_t num);

#endif