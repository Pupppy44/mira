# Set the NASM assembler
ASM = nasm

# Output files
BOOT_BIN = boot.bin
ASM_SRC = boot.asm

# Rule to build boot.bin from boot.asm
$(BOOT_BIN): $(ASM_SRC)
	$(ASM) -o $(BOOT_BIN) $(ASM_SRC)

# Clean rule to remove the generated boot.bin
clean:
	rm -f $(BOOT_BIN)

# Default target to build boot.bin
all: $(BOOT_BIN)