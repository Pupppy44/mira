#include "inc/mira.h"
#include <stdint.h>

#define VGA_WIDTH  320
#define VGA_HEIGHT 200
static volatile uint8_t* vga_buffer = (volatile uint8_t*)0xA0000;

int mk_entry() {
while (1) {
        for (int y = 0; y < 128; y++) {
        for (int x = 0; x < 128; x++) {
            // Bounds check (ensures image stays within VGA buffer)
            if (x >= VGA_WIDTH || y >= VGA_HEIGHT) {
                continue;
            }
            // Get the color index from the image data array
            uint8_t color_index = mo[y * 128 + x];

            // flip the byte (for example, white becomes black and vice versa)
            color_index = ~color_index;

            // Write the color index to the VGA buffer
            vga_buffer[y * VGA_WIDTH + x] = color_index;
        }
    }
}

    while (1);

    return 0; // success
}
