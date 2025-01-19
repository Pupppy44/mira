#ifndef KERNEL_IDT_H
#define KERNEL_IDT_H

#include <stdint.h>
#include "syscalls.h"

#define MK_CODE_SELECTOR 0x08
#define MK_DATA_SELECTOR 0x10

// Structure for an IDT entry
typedef struct {
    uint16_t offset_low;
    uint16_t segment_selector;
    uint8_t ist : 3;
    uint8_t reserved0 : 5;
    uint8_t type_attributes;
    uint16_t offset_middle;
    uint32_t offset_high;
    uint32_t reserved1;
} __attribute__((packed)) mk_idt_entry;

// Structure for the IDT pointer
typedef struct {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) mk_idt_ptr;

// Function to set an IDT entry
void mk_idt_set_entry(mk_idt_entry *entry, uintptr_t handler, uint16_t segment_selector, uint8_t type_attributes);

// Function to initialize the IDT
void mk_idt_init();

#endif