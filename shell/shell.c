// Mira User-Mode Shell Entry Point
int ms_entry() {
    *(unsigned char*)0xB8000 = 'M';
    *(unsigned char*)0xB8001 = 0x0F;

    return 0;
}