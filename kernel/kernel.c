#include "include/idt.h"
#include "include/tasks.h"
// Mira Kernel Entry Point
int mk_entry() {
    idt_init();

    unsigned char shellcode[] = { 0x48, 0xC7, 0xC0, 0x05, 0x00, 0x00, 0x00, 0x48, 0xC7, 0xC7, 0x03, 0x00, 0x00, 0x00, 0xCD, 0x80 };

    mk_task* task = mk_create_task(shellcode, sizeof(shellcode));
    mk_execute_task(task);

    while (1);

    return 0;
}