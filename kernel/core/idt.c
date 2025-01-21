#include "../include/idt.h"
#include "../include/tasks.h"

#define MK_IDT_ENTRIES 256
mk_idt_entry idt[MK_IDT_ENTRIES];
mk_idt_ptr idt_ptr;

char ralndChar() {
    // we must implement our own rand in here

    // this is a simple implementation of a random number generator

    static unsigned int seed = 0;
    seed = seed * 1103515245 + 12345;
    // RETURN A NUMBER BETWEEN 79 AND 159
    return (seed / 65536) % 80 + 79; // return a number between 0 and 255, which is a valid ASCII character
}
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static int current_task = 0;

// A data structure to store registers for each task (extensible for all regs)
typedef struct {
    uintptr_t rsp;
    // Optionally add rbx, rbp, r12, r13, r14, r15, etc. if needed
} regs_t;

// We'll assume each mk_task now has a regs field to store CPU state:
static regs_t g_task_contexts[32];
// Retrieve next task index in a round-robin manner
static int get_next_task(void) {
    int count = mk_get_task_count();
    if (count <= 0) {
        return -1; 
    }
    current_task = (current_task + 1) % count;
    return current_task;
}
void pit_timer_interrupt_handler(void *frame /* or interrupt frame pointer */)
{
            *(unsigned char*)0xB8006 = ralndChar();
        *(unsigned char*)0xB8007 = 0x03; // Set color
        
    // 1. Save registers of the current task
    //    Here, we only illustrate saving the stack pointer (rsp).
    //    In reality, you'd also push/preserve other registers in ASM.
    uintptr_t saved_rsp;
    __asm__ volatile ("movq %%rsp, %0" : "=r" (saved_rsp));
    int old_task = current_task;
    if (old_task >= 0) {
        g_task_contexts[old_task].rsp = saved_rsp;
    }

    // 2. Get next task index
    int next = get_next_task();
    if (next < 0) {
        // Acknowledge PIC if needed: outb(0x20, 0x20);
        return; 
    }

    // 3. Restore next task's registers (rsp) and jump there
    uintptr_t new_rsp = g_task_contexts[next].rsp;

    // If the new task is just starting, initialize it from tasks.c info:
    mk_task** all_tasks = mk_get_tasks();
    mk_task* next_task = all_tasks[next];
    if (new_rsp == 0) {
        // The task has not run yet. Set it to the top of its allocated stack.
        new_rsp = next_task->stack_ptr;
        g_task_contexts[next].rsp = new_rsp;
    }

    // Acknowledge the PIC (if using legacy PIC). Example:
    // outb(0x20, 0x20);

    // 4. Switch to the next task
    // Use IRET-based approach in real code or ring transitions. Here, a simplified ring-0 example:
    __asm__ volatile (
        "movq %0, %%rsp\n\t"   // Load next task RSP
        // The simplest approach is a "jmp" to the code, but if the code has jmp $,
        // we rely on the next timer interrupt to switch again.
        "jmp *%1\n\t"
        :
        : "r"(new_rsp), "r"(next_task->base)
        : "memory"
    );

    // Acknowledge the interrupt
    outb(0x20, 0x20);
}

// Define PIT ports
#define PIT_CHANNEL0_DATA 0x40
#define PIT_MODE_COMMAND  0x43
#define PIC_MASTER_COMMAND 0x20
#define PIC_MASTER_DATA    0x21

// Function to initialize the PIT
void init_pi2t(uint32_t frequency_hz) {
    // Calculate the divisor
    uint32_t divisor = 1193182 / frequency_hz;

    // Set the PIT command byte:
    // - Channel 0 (00)
    // - Access mode: lobyte/hibyte (11)
    // - Operating mode: Rate Generator (010) - generates a square wave
    // - Binary mode (0)
    outb(PIT_MODE_COMMAND, 0x34);

    // Send the low byte of the divisor
    outb(PIT_CHANNEL0_DATA, divisor & 0xFF);

    // Send the high byte of the divisor
    outb(PIT_CHANNEL0_DATA, (divisor >> 8) & 0xFF);

    // Unmask IRQ0 on the PIC (assuming it's connected to IRQ0)
    // Read the current IMR (Interrupt Mask Register)
    unsigned char imr = inb(PIC_MASTER_DATA);
    // Clear the bit corresponding to IRQ0 (bit 0) to unmask it
    outb(PIC_MASTER_DATA, imr & ~0x01);
}

// Mira Kernel IDT Set Entry
void mk_idt_set_entry(mk_idt_entry *entry, uintptr_t handler, uint16_t segment_selector, uint8_t type_attributes) {
    entry->offset_low = handler & 0xFFFF;
    entry->segment_selector = segment_selector;
    entry->ist = 0; // Use IST 0
    entry->reserved0 = 0;
    entry->type_attributes = type_attributes;
    entry->offset_middle = (handler >> 16) & 0xFFFF;
    entry->offset_high = (handler >> 32) & 0xFFFFFFFF;
    entry->reserved1 = 0;
}

// Mira Kernel IDT Loader
void mk_idt_load(mk_idt_ptr *idt_ptr) {
    //init_pi2t(10000); // 1000 Hz
    __asm__ volatile (
        "lidt %0"
        : : "m" (*idt_ptr)
    );
}

static int c = 0;


// Mira Kernel IDT Handler
void mk_idt_handler(void * frame) {
    __asm__ volatile ("cli"); // Disable interrupts

pit_timer_interrupt_handler(frame);

outb(0x20, 0x20); // Acknowledge the interrupt
    __asm__ volatile ("sti"); // Enable interrupts

}

void mk_idt_pit_handler(void* frame) {
    __asm__ volatile ("cli"); // Disable interrupts


    // Call the PIT timer interrupt handler
    pit_timer_interrupt_handler(frame);

     outb(0x20, 0x20); 

    __asm__ volatile ("sti"); // Enable interrupts
}

// Initialize the IDT
void mk_idt_init() {
    idt_ptr.limit = sizeof(mk_idt_entry) * MK_IDT_ENTRIES - 1;
    idt_ptr.base = (uint64_t)&idt;

    // Set all IDT entries to a default handler
    for (int i = 0; i < MK_IDT_ENTRIES; i++) {
        mk_idt_set_entry(&idt[i], (uintptr_t)mk_idt_handler, MK_CODE_SELECTOR, 0x8E); // Ring 0
    }

    // Set the IDT entry for the syscall interrupt (0x80)
    mk_idt_set_entry(&idt[0x80], (uintptr_t)mk_syscall_handler, MK_CODE_SELECTOR, 0xEE); // Ring 3

    // Set the IDT entry for the PIT timer interrupt (0x20)
    mk_idt_set_entry(&idt[0x20], (uintptr_t)mk_idt_pit_handler, MK_CODE_SELECTOR, 0x8E); // Ring 0

    // Load the IDT
    mk_idt_load(&idt_ptr);
}