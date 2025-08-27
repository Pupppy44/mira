#include "../inc/vbe.h"

// ? These variables store the graphics mode information after
// ? initialization for fast access during drawing operations.
static volatile uint32_t* framebuffer;
static uint16_t screen_width;
static uint16_t screen_height;
static uint16_t pitch;

// Mira Kernel VBE Initialization
void mk_vbe_init(void) {
    volatile mk_boot_info* boot_info = (mk_boot_info*)0x1000; // Boot info at 0x1000

    framebuffer = (uint32_t*)boot_info->framebuffer_address;
    screen_width = boot_info->screen_width;
    screen_height = boot_info->screen_height;
    pitch = boot_info->pitch;
}

// Mira Kernel VBE Get Screen Width
uint16_t mk_vbe_get_width(void) {
    return screen_width;
}

// Mira Kernel VBE Get Screen Height
uint16_t mk_vbe_get_height(void) {
    return screen_height;
}

// Mira Kernel VBE Draw Pixel
void mk_vbe_draw_pixel(int x, int y, uint32_t color) {
    if (x >= 0 && x < screen_width && y >= 0 && y < screen_height) {
        // ? The pitch is the number of bytes per horizontal line. To
        // ? get the number of 32-bit pixels in a line, we divide by 4.
        uint32_t pitch_in_pixels = pitch / 4;
        framebuffer[y * pitch_in_pixels + x] = color;
    }
}

// Mira Kernel VBE Draw Image
void mk_vbe_draw_image(int x, int y, uint16_t width, uint16_t height, uint32_t* data) {
    uint32_t pitch_in_pixels = pitch / 4;

    const uint32_t* src = data;
    volatile uint32_t* dest = framebuffer + (y * pitch_in_pixels) + x;

    for (uint16_t row = 0; row < height; ++row) {
        // Copy one row of pixels from src to dest.
        for (uint16_t col = 0; col < width; ++col) {
            dest[col] = src[col];
        }

        // Advance pointers to the next row.
        src += width;
        dest += pitch_in_pixels;
    }
}