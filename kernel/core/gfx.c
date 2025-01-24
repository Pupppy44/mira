#include <stdint.h>
#include "../inc/gfx.h"

// Port I/O in 32/64-bit mode (inline assembly)
static inline void outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

// ─────────────────────────────────────────────────────
// 1) VGA 13h 256-Color Palette
//    We provide a typical 256-color palette:
//    - Indices 0–15 are the EGA defaults
//    - Indices 16–231 form a 6x6x6 color cube
//    - Indices 232–255 are grayscale
// ─────────────────────────────────────────────────────
uint8_t mk_gfx_colors[256][3] = {
    // 0–15: EGA palette
    {0,   0,   0},   {0,   0,   170}, {0,   170, 0},   {0,   170, 170},
    {170, 0,   0},   {170, 0,   170}, {170, 85,  0},   {170, 170, 170},
    {85,  85,  85},  {85,  85,  255}, {85,  255, 85},  {85,  255, 255},
    {255, 85,  85},  {255, 85,  255}, {255, 255, 85},  {255, 255, 255}
};

// For 16..231: build a 6×6×6 color cube
// For 232..255: grayscale ramp
// We'll fill them in a global init function (or here statically).
// This approach uses 51 = 255/5 increments for a 6×6×6 cube.
static void mk_init_color_array(void) {
    int index = 16;
    for (int r = 0; r < 6; r++) {
        for (int g = 0; g < 6; g++) {
            for (int b = 0; b < 6; b++) {
                mk_gfx_colors[index][0] = (uint8_t)(r * 51);
                mk_gfx_colors[index][1] = (uint8_t)(g * 51);
                mk_gfx_colors[index][2] = (uint8_t)(b * 51);
                index++;
            }
        }
    }
    // 232..255: grayscale steps
    for (int i = 0; i < 24; i++) {
        uint8_t val = (uint8_t)(i * 11);
        mk_gfx_colors[232 + i][0] = val;
        mk_gfx_colors[232 + i][1] = val;
        mk_gfx_colors[232 + i][2] = val;
    }
}

// ─────────────────────────────────────────────────────
// 2) mk_gfx_map_colors
//    Write our 256 R,G,B values to the VGA DAC using
//    port 0x3C8 (color index) and 0x3C9 (RGB data).
// ─────────────────────────────────────────────────────
void mk_gfx_map_colors(void) {
    mk_init_color_array();

    // Each color component is 6 bits in standard VGA (0..63),
    // so we shift from 0..255 down to 0..63 by >> 2.
    for (int i = 0; i < 256; i++) {
        outb(0x3C8, i);
        outb(0x3C9, mk_gfx_colors[i][0] >> 2);
        outb(0x3C9, mk_gfx_colors[i][1] >> 2);
        outb(0x3C9, mk_gfx_colors[i][2] >> 2);
    }
}

// ─────────────────────────────────────────────────────
// 3) mk_gfx_draw_bitmap
//    Draw an indexed-color bitmap in 320×200 Mode 13h
//    at screen coordinate (x, y).
// ─────────────────────────────────────────────────────
void mk_gfx_draw_bitmap(const uint8_t* bitmap, int width, int height, int x, int y) {
    // Mode 13h: Segment A0000 is 320×200, 1 byte/pixel
    volatile uint8_t* video = (uint8_t*)0xA0000;
    int screenWidth = 320; // Mode 13h

    // Loop through each pixel of the bitmap
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            // Color index in the bitmap
            uint8_t colorIndex = bitmap[row * width + col];
            // For mode 13h, the offset in the linear buffer is row * 320 + col
            int offset = (y + row) * screenWidth + (x + col);

            // If we want to do clipping, we'd check offset or x,y bounds here
            video[offset] = colorIndex;
        }
    }
}