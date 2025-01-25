#ifndef GFX_H
#define GFX_H

#include <stdint.h>

#define MK_GFX_VGA_WIDTH 320
#define MK_GFX_VGA_HEIGHT 200
#define MK_GFX_VGA_ADDRESS 0xA0000

// Mira Kernel GFX Dimensions
typedef struct {
    int x;
    int y;
    int width;
    int height;
} mk_gfx_dimensions_t;

// Mira Kernel GFX Clear Screen
void mk_gfx_clear_screen(uint8_t color);

// Mira Kernel GFX Set Pixel
void mk_gfx_set_pixel(int x, int y, uint8_t color);

// Mira Kernel GFX Set Pixels
void mk_gfx_set_pixels(uint8_t *pixels, mk_gfx_dimensions_t *dimensions);

#endif