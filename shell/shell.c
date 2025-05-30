#include <stdint.h>

void ms_entry() {
    asm volatile (
        "mov   $1, %rax      \n\t"  // syscall #1 = sys_print
        "mov   $'M', %rdi    \n\t"  // character byte
        "mov   $0x07, %rsi   \n\t"  // color byte
        "int   $0x80         \n\t"  // call into mk_syscall_handler
    );
}
