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

static inline void* memcpy(void *dst, const void *src, size_t n) {
    uint8_t *d = (uint8_t *)dst;
    const uint8_t *s = (const uint8_t *)src;
    while (n--) *d++ = *s++;
    return dst;
}

static inline int memcmp(const void *a, const void *b, size_t n) {
    const uint8_t *pa = (const uint8_t *)a;
    const uint8_t *pb = (const uint8_t *)b;
    while (n--) {
        if (*pa != *pb) return (int)(*pa - *pb);
        ++pa; ++pb;
    }
    return 0;
}

#endif
