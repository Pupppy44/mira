#ifndef MK_GDT_H
#define MK_GDT_H

#include <stdint.h>

// A single 8-byte GDT “code/data” entry
typedef struct __attribute__((packed)) {
    uint16_t limit_low;    // bits 0–15 of segment limit
    uint16_t base_low;     // bits 0–15 of base address
    uint8_t  base_mid;     // bits 16–23 of base address
    uint8_t  access;       // P, DPL, S=1, type bits
    uint8_t  granularity;  // flags (L, G, etc) + limit[19:16]
    uint8_t  base_high;    // bits 24–31 of base address
} mk_gdt_entry_t;

// A 16-byte TSS descriptor spans two consecutive GDT slots
typedef struct __attribute__((packed)) {
    uint16_t length;     // Limit[15:0]
    uint16_t base_low;   // Base[15:0]
    uint8_t  base_mid1;  // Base[23:16]
    uint8_t  flags1;     // Access: P=1, DPL=0, Type=1001b (64-bit TSS)
    uint8_t  flags2;     // Flags2: Limit[19:16] & G/L bits
    uint8_t  base_mid2;  // Base[31:24]
    uint32_t base_high;  // Base[63:32]
    uint32_t reserved;   // Must be zero
} mk_tss_desc_t;

// Actual 64-bit TSS structure: only RSP0 matters here
typedef struct __attribute__((packed)) {
    uint32_t reserved0;
    uint64_t rsp0;       // Ring-0 stack pointer
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved1;
    uint64_t ist[7];
    uint64_t reserved2;
    uint16_t reserved3;
    uint16_t io_map_base;
} mk_tss_t;

// “LGDT” pointer
typedef struct __attribute__((packed)) {
    uint16_t limit;
    uint64_t base;
} mk_gdt_ptr_t;

#define MK_GDT_NUM_ENTRIES 7

// GDT indices (each index × 8 is the selector base)
#define MK_KERNEL_CODE_INDEX 1
#define MK_KERNEL_DATA_INDEX 2
#define MK_USER_CODE_INDEX   3
#define MK_USER_DATA_INDEX   4
#define MK_TSS_INDEX         5

// Selector values = (index << 3) | RPL
#define MK_KERNEL_CODE_SELECTOR ((MK_KERNEL_CODE_INDEX << 3) | 0)
#define MK_KERNEL_DATA_SELECTOR ((MK_KERNEL_DATA_INDEX << 3) | 0)
#define MK_USER_CODE_SELECTOR   ((MK_USER_CODE_INDEX   << 3) | 3)
#define MK_USER_DATA_SELECTOR   ((MK_USER_DATA_INDEX   << 3) | 3)
#define MK_TSS_SELECTOR         ((MK_TSS_INDEX         << 3) | 0)

// Mira Kernel GDT Initialization
void mk_gdt_init(void);

// Mira TSS structure
extern mk_tss_t mk_tss;

#endif // MK_GDT_H
