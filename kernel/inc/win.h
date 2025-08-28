#ifndef MIRA_WIN_H
#define MIRA_WIN_H

#include "mem.h"

#define MK_WINDOWS_MAX 32

typedef struct {
    int id;
    int x;
    int y;
    int width;
    int height;
} mk_window;

// Mira Kernel Create Window
mk_window* mk_create_window(int x, int y, int width, int height);

// Mira Kernel Update Window
void mk_update_window(int window_id, uint32_t* framebuffer);

#endif // MIRA_WIN_H