// Mira About Shell Application
// Displays a window with information
// about the operating system, Mira.

#include "../tests/mira_ui.h"

void main() {
    // Create a window for the "About" information
    mira_ui_window_t about_window = mira_ui_create_window("About Mira", 400, 300);
    
    // Draw the content inside the window
    mira_ui_draw_text(&about_window, "Mira Operating System", 10, 10);
    mira_ui_draw_text(&about_window, "Version: 1.0", 10, 30);
    
    // Display the window on the desktop
    mira_ui_display_window(&about_window);
    
    // Wait for user input to close the window
    char key;
    do {
        key = mira_ui_get_input(&about_window);
    } while (key != 'q'); // Press 'q' to close the window
    
    // Close the window
    mira_ui_close_window(&about_window);
}