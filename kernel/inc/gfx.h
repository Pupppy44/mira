#ifndef GFX_H
#define GFX_H

#include <stdint.h>

// A 256-element array of 3-byte colors (R, G, B).
extern uint8_t mk_gfx_colors[256][3];

// Initializes the VGA 13h 256-color palette via outb().
void mk_gfx_map_colors(void);

// Draws a bitmap (indexed-color array) at position (x, y) in 320×200 Mode 13h.
//   - bitmap: pointer to the array of color indices
//   - width, height: dimensions of the bitmap
//   - x, y: on-screen location (top-left corner)
void mk_gfx_draw_bitmap(const uint8_t* bitmap, int width, int height, int x, int y);

#endif // GFX_H