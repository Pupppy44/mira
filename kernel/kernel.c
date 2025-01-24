#include "inc/mira.h"
#include "inc/gfx.h"
#include <stdint.h>

#define VGA_WIDTH 320
#define VGA_HEIGHT 200
#define VGA_ADDRESS 0xA0000

// Function to set a pixel at (x, y) with a given color index
void set_pixel(int x, int y, uint8_t color) {
    if (x < 0 || x >= VGA_WIDTH || y < 0 || y >= VGA_HEIGHT) {
        return; // Out of bounds
    }
    uint8_t *vga = (uint8_t*)VGA_ADDRESS;
    vga[y * VGA_WIDTH + x] = color;
}

void draw_image() {
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            set_pixel(x, y, image_data[y][x]);
        }
    }
}

int mk_entry() {
   // mk_gfx_map_colors();
    draw_image();

    while (1);

    return 0; // success
}
