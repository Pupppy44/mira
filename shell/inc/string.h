#ifndef MINI_STRING_H
#define MINI_STRING_H

#include <stddef.h>
#include <stdint.h>

static inline size_t strlen(const char *s) {
    size_t n = 0;
    while (s[n]) ++n;
    return n;
}

static inline int strcmp(const char *a, const char *b) {
    while (*a && (*a == *b)) { ++a; ++b; }
    return (uint8_t)*a - (uint8_t)*b;
}

static inline void* memset(void *dst, int v, size_t n) {
    uint8_t *p = (uint8_t *)dst;
    while (n--) *p++ = (uint8_t)v;
    return dst;
}

#endif
