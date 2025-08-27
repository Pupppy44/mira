#ifndef MK_VBE_H
#define MK_VBE_H

#include <stdint.h>

// Mira Kernel Boot Information
// * This internal structure must exactly match the memory layout from the bootloader.
typedef struct __attribute__((packed)) {
    uint64_t framebuffer_address;
    uint16_t screen_width;
    uint8_t  _padding1[2];
    uint16_t screen_height;
    uint8_t  _padding2[2];
    uint16_t pitch;
    uint8_t  _padding3[3];
    uint8_t  bpp;
} mk_boot_info;

// Mira Kernel VBE Initialization
// Reads the boot information and sets up the internal graphics context.
void mk_vbe_init(void);

// Mira Kernel VBE Get Screen Width
uint16_t mk_vbe_get_width(void);

// Mira Kernel VBE Get Screen Height
uint16_t mk_vbe_get_height(void);

// Mira Kernel VBE Draw Pixel
// Draws a single 32-bit color pixel at the specified (x, y) coordinate.
void mk_vbe_draw_pixel(int x, int y, uint32_t color);

// Mira Kernel VBE Draw Image
// Renders a raw 32-bit image buffer to the screen at a given position.
void mk_vbe_draw_image(int x, int y, uint16_t width, uint16_t height, uint32_t* data);

#endif // MK_VBE_H