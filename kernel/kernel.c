#include "inc/mira.h"
#include "inc/gfx.h"
#include <stdint.h>

#define VGA_WIDTH 320
#define VGA_HEIGHT 200
#define VGA_ADDRESS 0xA0000

// Function to set a pixel at (x, y) with a given color index
void set_pixel(int x, int y, uint8_t color) {
    uint8_t *vga = (uint8_t*)VGA_ADDRESS;
    vga[y * VGA_WIDTH + x] = color;
}

void draw_image() {
    for (int y = 0; y < 128; y++) {
        for (int x = 0; x < 128; x++) {
            set_pixel(x, y, image_data[y * 128 + x]);
        }
    }
}

int mk_entry() {
    // set_pixel(0, 0, 1);
    // while (1);
    draw_image();

    while (1);

    return 0; // success
}
