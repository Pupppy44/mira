# Set the C compiler and debugger
CC = clang
GDB = gdb 

LD = ld.lld
ASM = nasm

INC = -I/inc
SRC = $(shell find . -type f -name "*.c")
ASM_SRC = $(shell find . -type f -name "*.asm")

# CRITICAL: assembly must be linked first
OBJ = ${ASM_SRC:.asm=.o} ${SRC:.c=.o} 

CCFLAGS = -Iinc -ggdb -c -ffreestanding -target x86_64-none-elf -w
LDFLAGS = -Ttext 0x8200
LDFLAGS_BIN = ${LDFLAGS} --oformat binary
ASFLAGS = -f elf64

# Default target for shell (produce shell.bin instead of kernel)
all: shell.bin

# Build shell.bin (binary format)
shell.bin: ${OBJ}
	${LD} -o $@ ${LDFLAGS_BIN} $^

# Optional: Keep shell.elf for debugging or other purposes
shell.elf: ${OBJ}
	${LD} -o $@ ${LDFLAGS} $^

%.o: %.c
	${CC} ${CCFLAGS} ${INC} -o $@ $^

%.o: %.asm
	${ASM} $< ${ASFLAGS} -o $@

clean:
	rm -f shell.elf *.o *.bin **/*.o