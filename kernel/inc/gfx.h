#ifndef GFX_H
#define GFX_H

#include <stdint.h>

#define VGA_WIDTH  320
#define VGA_HEIGHT 200

// A pointer to the VGA Mode 13h base address (8-bit color)
static volatile uint8_t* vga_buffer = (volatile uint8_t*)0xA0000;

/**
 * put_pixel:
 *   Sets an 8-bit pixel at (x, y) to 'color'.
 */
void put_pixel(int x, int y, uint8_t color);

/**
 * draw_line:
 *   Draws a line from (x0, y0) to (x1, y1) using a simple Bresenham's algorithm.
 */
void draw_line(int x0, int y0, int x1, int y1, uint8_t color);

/**
 * draw_rect:
 *   Draws a rectangle outline at (x, y) with width w and height h.
 */
void draw_rect(int x, int y, int w, int h, uint8_t color);

/**
 * fill_rect:
 *   Fills a rectangle at (x, y) with width w and height h.
 */
void fill_rect(int x, int y, int w, int h, uint8_t color);

/**
 * draw_rounded_rect:
 *   Draws a rectangle with rounded corners. Uses simplistic "antialiasing"
 *   by drawing semi-transparent edges. In true 8-bit VGA, "semi-transparent"
 *   is approximate (we just use a different color).
 *
 *   corner_radius: radius of the rounded corner
 *   main_color: the main rectangle border color
 *   aa_color: a slightly different color used for "blending"
 */
void draw_rounded_rect(int x, int y, int w, int h, int corner_radius,
                       uint8_t main_color, uint8_t aa_color);

/**
 * draw_bmp:
 *   Draws a raw 8-bit BMP-like image array at (dest_x, dest_y).
 *   'bmp_data' pointer points to the start of the BMP pixels (already uncompressed).
 *   'bmp_width', 'bmp_height' specify the image size in pixels.
 *
 *   Because we are in 8-bit mode, we assume bmp_data is also in the same color
 *   indexing format. Real BMP files have headers/palettes, so you'd parse that 
 *   in practice. This example just copies data directly.
 */
void draw_bmp(int dest_x, int dest_y,
              const uint8_t* bmp_data, int bmp_width, int bmp_height);

#endif // GFX_H
