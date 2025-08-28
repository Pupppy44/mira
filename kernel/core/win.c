#include "../inc/win.h"
#include "../inc/vbe.h"

static mk_window* mk_windows[MK_WINDOWS_MAX] = {0};
static int mk_windows_count = 0;

// Mira Kernel Create Window
mk_window* mk_create_window(int x, int y, int width, int height) {
    if (mk_windows_count >= MK_WINDOWS_MAX) {
        return NULL;
    }

    mk_window* window = (mk_window*)mk_malloc(sizeof(mk_window));

    if (!window) {
        return NULL;
    }

    window->id = mk_windows_count++;
    window->x = x;
    window->y = y;
    window->width = width;
    window->height = height;

    mk_windows[window->id] = window;
    return window;
}

void mk_update_window(int window_id, uint32_t* framebuffer) {
    mk_window* window = mk_windows[window_id];

    // We can directly take the updated framebuffer and
    // draw it to the screen via Mira's VBE functions.
    for (int i = 0; i < window->height; i++) {
        for (int j = 0; j < window->width; j++) {
            uint32_t color = framebuffer[i * window->width + j];
            mk_vbe_draw_pixel(window->x + j, window->y + i, color);
        }
    }
}