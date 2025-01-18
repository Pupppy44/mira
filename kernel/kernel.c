int mk_main() {
    char *vga = (char *)0xb8000;
    char *str = "mira!";

    // clear the screen first
    for (int i = 0; i < 80 * 25; i++) {
        *vga++ = ' ';
        *vga++ = 0x07;
    }

    // reset cursor
    vga = (char *)0xb8000;

    // print the string to the screen
    for (int i = 0; i < 5; i++) {
        *vga++ = *str++;
        *vga++ = 0x05; // this is the color : 0x05 is purple on black
    }

    while (1);
    
    return 0;
}