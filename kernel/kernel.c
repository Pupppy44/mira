// Define the assembly shellcode to write purple 'X' on black
unsigned char shellcode[] = { 0x48, 0xC7, 0xC0, 0x00, 0x80, 0x0B, 0x00, 0x66, 0xC7, 0x00, 0x0E, 0x48, 0xC3 };

// Function to execute assembly code at a given address
typedef void (*asm_func_ptr)(void);

void execute_assembly(unsigned long address) {
    asm_func_ptr func = (asm_func_ptr)address;
    func();
}

// Kernel entry point (assuming linker places this at 0x1000)
int mk_entry() {
    // clear the screen
    for (int i = 0; i < 80 * 25 * 2; i++) {
        unsigned char *video = (unsigned char *)0xB8000;
        video[i] = 0;
    }

    // text mode
    unsigned char *video = (unsigned char *)0xB8000;
    video[0] = 'M'; // Character
    video[1] = 0x05; // Color
    video[2] = 'i'; // Character
    video[3] = 0x05; // Color
    video[4] = 'r'; // Character
    video[5] = 0x05; // Color
    video[6] = 'a'; // Character
    video[7] = 0x05; // Color
    video[8] = '!'; // Character
    video[9] = 0x05; // Color
    
    // Copy the shellcode to the designated memory address (0x1100)
    unsigned char *asm_address = (unsigned char *)0x1100;
    for (int i = 0; i < sizeof(shellcode); i++) {
        asm_address[i] = shellcode[i];
    }

    // Execute the assembly code
    execute_assembly(0x1100);

    // Halt the kernel
    while (1);

    return 0;
}