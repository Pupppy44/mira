#include "inc/mira.h"

// Example of creating a window and updating it
// Soon, the shell will essentially be a desktop!

/*
 * TODOs:
 * - Create a system call for running a new task from user space
 * - Create a system call for getting an asset from assets.bin into user space
 * - Create Mira2D, a 2D graphics library for rectangles, images, and text
 * - Implement the shell, a basic desktop with a taskbar and app icons
 * - Implement opening new applications through the app icons
 * - Create a system call for moving windows
 * - Implement window dragging, where the shell manages mouse input
 * - Add frames to windows with borders, title bars, and buttons
 */

void ms_entry(void) {
    int window_id = mira_create_window(20, 20, 10, 10);
    uint32_t framebuffer[10 * 10];

    for (int i = 0; i < 10 * 10; i++) {
        framebuffer[i] = 0x00FFFF00;
    }

    mira_update_window(window_id, framebuffer);

    while (1) {};
}
