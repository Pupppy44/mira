#include "inc/mira.h"
#include "inc/string.h"

#define MS_CMD_BUFFER_MAX 128 // Maximum command length

// Mira Shell Command Buffer & Length
static char ms_command_buffer[MS_CMD_BUFFER_MAX];
static size_t ms_command_length = 0; 

// Helper function to convert an integer to a string for printing.
static void itoa(int n, char* buffer) {
    if (n == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }

    int i = 0;
    int is_negative = 0;
    if (n < 0) {
        is_negative = 1;
        n = -n;
    }

    while (n != 0) {
        int rem = n % 10;
        buffer[i++] = rem + '0';
        n = n / 10;
    }

    if (is_negative) {
        buffer[i++] = '-';
    }

    for (int start = 0, end = i - 1; start < end; start++, end--) {
        char temp = buffer[start];
        buffer[start] = buffer[end];
        buffer[end] = temp;
    }
    
    buffer[i] = '\0';
}

// Mira Shell Echo Character
void ms_echo_char(char character) {
    char temp[2] = { character, 0 };
    mira_print(temp, 0x07);
}

// Mira Shell Process Line
void ms_process_line(void) {
    ms_command_buffer[ms_command_length] = '\0';

    if (strcmp(ms_command_buffer, "HELP") == 0) {
        mira_print(
            "Available commands:\n"
            "help  - show this help message\n"
            "about - show details about this shell\n"
            "mouse - get current mouse state\n", 0x07
        );
    } else if (strcmp(ms_command_buffer, "ABOUT") == 0) {
        mira_print(
            "Mira Shell v1.0\n"
            "A simple shell for the Mira operating system.\n", 0x07
        );
    } else if (strcmp(ms_command_buffer, "MOUSE") == 0) {
        mira_mouse_state_t state;
        mira_get_mouse_state(&state);

        char x_buf[12];
        char y_buf[12];
        itoa(state.x, x_buf);
        itoa(state.y, y_buf);

        mira_print("Mouse State -> X: ", 0x07);
        mira_print(x_buf, 0x07);
        mira_print(", Y: ", 0x07);
        mira_print(y_buf, 0x07);
        mira_print(", L: ", 0x07);
        mira_print(state.left_button ? "1" : "0", 0x07);
        mira_print(", R: ", 0x07);
        mira_print(state.right_button ? "1" : "0", 0x07);
        mira_print(", M: ", 0x07);
        mira_print(state.middle_button ? "1" : "0", 0x07);
        mira_print("\n", 0x07);

    } else if (ms_command_length != 0) {
        mira_print("Unknown command: ", 0x07);
        mira_print(ms_command_buffer, 0x07);
        mira_print("\n", 0x07);
    }

    ms_command_length = 0;
}


void ms_entry(void) {
    mira_print(
        "Welcome to the Mira shell!\n" 
        "Type 'help' for a list of commands.\n", 0x07
    );

    for (;;) {
        char key = mira_get_key();

        // Ignore invalid keys
        if (key == 0) {
            continue;
        }

        // Process input when the user presses enter
        if (key == '\n' || key == '\r') {
            ms_echo_char('\n');
            ms_process_line();
            continue;
        }

        // Support for backspace and delete
        if ((key == '\b' || key == 0x7F) && ms_command_length) {
            --ms_command_length;

            // Echo backspace and clear the last character
            ms_echo_char('\b'); 
            ms_echo_char(' '); 
            ms_echo_char('\b');

            continue;
        }

        // Ignore control characters and non-printable characters
        if (key >= 32 && key < 127 && ms_command_length + 1 < MS_CMD_BUFFER_MAX) {
            ms_command_buffer[ms_command_length++] = key;
            ms_echo_char(key);
        }
    }
}