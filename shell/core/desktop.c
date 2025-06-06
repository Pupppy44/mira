// Mira Shell Desktop Entry
// This file handles the visual desktop for the Mira Shell.
// It renders the desktop background, taskbar, and icons.

#include "../tests/mira_ui.h"

void main() {
    // Create a desktop window
    mira_ui_window_t desktop_window = mira_ui_create_window("Mira Desktop", 800, 600);

    // Draw a colored background for the desktop
    for (int y = 0; y < desktop_window.height; y++) {
        for (int x = 0; x < desktop_window.width; x++) {
            mira_ui_draw_pixel(&desktop_window, x, y, 0x1A);
        }
    }

    // Draw a simple taskbar at the bottom of the desktop
    for (int x = 0; x < desktop_window.width; x++) {
        mira_ui_draw_pixel(&desktop_window, x, desktop_window.height - 20, 0x04);
    }
    
    // Draw the desktop text
    mira_ui_draw_text(&desktop_window, "Welcome to Mira Desktop", 10, 10);
    
    // Display the desktop window
    mira_ui_display_window(&desktop_window);

    // Wait for user input to close the desktop
    char key;
    do {
        key = mira_ui_get_input(&desktop_window);
    } while (key != 'q'); // Press 'q' to close the desktop
    
    // Close the desktop window
    mira_ui_close_window(&desktop_window);
}