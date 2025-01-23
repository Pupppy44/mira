/*
 * gfx.c - Example VGA Mode 13h drawing in x64 kernel mode
 *
 * NOTE:
 * 1. This code assumes that the linear memory at 0xA0000 (Mode 13h buffer)
 *    is directly accessible in your kernel environment.
 * 2. In real x64 environments, you must map this region into your page tables,
 *    because 0xA0000 is normally outside standard paging. 
 * 3. All functions here assume an 8-bit 320×200 Mode 13h-style framebuffer.
 * 4. This example uses simplified antialiasing for rounded corners. Real 
 *    anti-aliasing (e.g., Wu's line algorithm) may be more complex.
 */

#include "../inc/gfx.h"

/**
 * put_pixel:
 *   Sets an 8-bit pixel at (x, y) to 'color'.
 */
void put_pixel(int x, int y, uint8_t color) {
    // Bounds checking
    if (x < 0 || x >= VGA_WIDTH || y < 0 || y >= VGA_HEIGHT) {
        return; // Outside screen, no draw
    }
    // Calculate offset and set color
    vga_buffer[y * VGA_WIDTH + x] = color;
}

/**
 * draw_line:
 *   Draws a line from (x0, y0) to (x1, y1) using a simple Bresenham's algorithm.
 */
void draw_line(int x0, int y0, int x1, int y1, uint8_t color) {
    int dx = x1 - x0;
    int dy = y1 - y0;

    int abs_dx = (dx > 0) ? dx : -dx;
    int abs_dy = (dy > 0) ? dy : -dy;
    int sx = (dx > 0) ? 1 : -1;
    int sy = (dy > 0) ? 1 : -1;
    int err = abs_dx - abs_dy;

    while (1) {
        put_pixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -abs_dy) {
            err -= abs_dy;
            x0 += sx;
        }
        if (e2 < abs_dx) {
            err += abs_dx;
            y0 += sy;
        }
    }
}

/**
 * draw_rect:
 *   Draws a rectangle outline at (x, y) with width w and height h.
 */
void draw_rect(int x, int y, int w, int h, uint8_t color) {
    // Top edge
    draw_line(x,        y,        x + w - 1, y,         color);
    // Bottom edge
    draw_line(x,        y + h - 1, x + w - 1, y + h - 1, color);
    // Left edge
    draw_line(x,        y,        x,         y + h - 1, color);
    // Right edge
    draw_line(x + w - 1, y,        x + w - 1, y + h - 1, color);
}

/**
 * fill_rect:
 *   Fills a rectangle at (x, y) with width w and height h.
 */
void fill_rect(int x, int y, int w, int h, uint8_t color) {
    for (int row = 0; row < h; row++) {
        for (int col = 0; col < w; col++) {
            put_pixel(x + col, y + row, color);
        }
    }
}

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
                       uint8_t main_color, uint8_t aa_color) {
                        aa_color = main_color;
    // Draw the regular rectangle minus corners
    // top edge
    draw_line(x + corner_radius, y, x + w - corner_radius - 1, y, main_color);
    // bottom edge
    draw_line(x + corner_radius, y + h - 1, x + w - corner_radius - 1,
              y + h - 1, main_color);
    // left edge
    draw_line(x, y + corner_radius, x, y + h - 1 - corner_radius, main_color);
    // right edge
    draw_line(x + w - 1, y + corner_radius, x + w - 1,
              y + h - 1 - corner_radius, main_color);

    // Approximate rounded corners with a simple quarter-circle approach
    int r_sq = corner_radius * corner_radius; // radius squared
    for (int cy = 0; cy < corner_radius; cy++) {
        for (int cx = 0; cx < corner_radius; cx++) {
            int dist_sq = cx * cx + cy * cy;
            if (dist_sq <= r_sq) {
                // Top-left corner
                put_pixel(x + corner_radius - 1 - cx, y + corner_radius - 1 - cy,
                          main_color);
                // Top-right corner
                put_pixel(x + w - corner_radius + cx, y + corner_radius - 1 - cy,
                          main_color);
                // Bottom-left corner
                put_pixel(x + corner_radius - 1 - cx, y + h - corner_radius + cy,
                          main_color);
                // Bottom-right corner
                put_pixel(x + w - corner_radius + cx, y + h - corner_radius + cy,
                          main_color);
            } else if (dist_sq <= (r_sq + corner_radius * 2)) {
                // Example "antialias" region (just use aa_color)
                put_pixel(x + corner_radius - 1 - cx, y + corner_radius - 1 - cy,
                          aa_color);
                put_pixel(x + w - corner_radius + cx, y + corner_radius - 1 - cy,
                          aa_color);
                put_pixel(x + corner_radius - 1 - cx, y + h - corner_radius + cy,
                          aa_color);
                put_pixel(x + w - corner_radius + cx, y + h - corner_radius + cy,
                          aa_color);
            }
        }
    }
}

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
              const uint8_t* bmp_data, int bmp_width, int bmp_height) {
    for (int row = 0; row < bmp_height; row++) {
        for (int col = 0; col < bmp_width; col++) {
            uint8_t color = bmp_data[row * bmp_width + col];
            put_pixel(dest_x + col, dest_y + row, color);
        }
    }
}

/**
 * Example usage in kernel mode (pseudo-code):
 * 
 * void some_kernel_init() {
 *     // Switch to VGA mode 13h if not already done (not shown here).
 *     // Clear screen:
 *     for(int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
 *         vga_buffer[i] = 0; // black
 *     }
 *
 *     // Draw a filled rectangle in color 10 (arbitrary index)
 *     fill_rect(50, 50, 100, 50, 10);
 *
 *     // Draw a red line
 *     draw_line(0, 0, 319, 199, 12);
 *
 *     // Draw a rounded rect with radius=10, main_color=14, aa_color=9
 *     draw_rounded_rect(60, 60, 80, 40, 10, 14, 9);
 *
 *     // Suppose we have a small 8-bit image data array, 16x16
 *     // const uint8_t bmp_data[16 * 16] = { ... };
 *     // draw_bmp(100, 100, bmp_data, 16, 16);
 * }
 */