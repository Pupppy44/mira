#include "../inc/gfx.h"

void mk_gfx_clear_screen(uint8_t color) {
    uint8_t *vga = (uint8_t*)MK_GFX_VGA_ADDRESS;

    // Set all pixels to the specified color (clear the screen)
    for (int i = 0; i < MK_GFX_VGA_WIDTH * MK_GFX_VGA_HEIGHT; i++) {
        vga[i] = color;
    }
}

void mk_gfx_set_pixel(int x, int y, uint8_t color) {
    uint8_t *vga = (uint8_t*)MK_GFX_VGA_ADDRESS;
    vga[y * MK_GFX_VGA_WIDTH + x] = color;
}

void mk_gfx_set_pixels(uint8_t *pixels, mk_gfx_dimensions_t *dimensions) {
    int x = dimensions->x;
    int row = dimensions->y; // Starting row
    int width = dimensions->width;
    int height = dimensions->height;

    for (int i = 0; i < width * height; i++) {
        // Get and set the pixel color
        uint8_t color = pixels[i];
        mk_gfx_set_pixel(x + (i % width), row, color);

        // If we reach the end of the row, move to the next row
        if (i % width == height - 1) {
            row++;
        }
    }
}
