#include "../inc/keyboard.h"
#include "../inc/util.h"

// Mira Kernel Keyboard Wait For Send Ready
// Waits until the keyboard controller's input buffer is empty.
static void mk_kbd_wait_for_send_ready() {
    while (mk_util_inb(MK_KBD_STATUS_PORT) & MK_KBD_STAT_IBF);
}

// Mira Kernel Keyboard Wait For Receive Ready
// Waits until the keyboard controller's output buffer is full.
static void mk_kbd_wait_for_recv_ready() {
    while (!(mk_util_inb(MK_KBD_STATUS_PORT) & MK_KBD_STAT_OBF));
}

// Mira Kernel Keyboard Send Command
// Sends a command byte to the PS/2 controller command port.
void mk_kbd_send_command(uint8_t cmd) {
    mk_kbd_wait_for_send_ready();
    mk_util_outb(MK_KBD_CMD_PORT, cmd);
}

// Mira Kernel Keyboard Send Data
// Sends a data byte to the PS/2 controller data port.
void mk_kbd_send_data(uint8_t data) {
    mk_kbd_wait_for_send_ready();
    mk_util_outb(MK_KBD_DATA_PORT, data);
}

// Mira Kernel Keyboard Read Data
// Reads a data byte from the PS/2 controller data port.
uint8_t mk_kbd_read_data() {
    mk_kbd_wait_for_recv_ready();
    return mk_util_inb(MK_KBD_DATA_PORT);
}

// Mira Kernel Keyboard Initialize
// Performs basic initialization for the keyboard.
void mk_keyboard_init() {
    // Reset the keyboard controller
    while(mk_util_inb(MK_KBD_STATUS_PORT) & MK_KBD_STAT_OBF) {
        mk_util_inb(MK_KBD_DATA_PORT);
        mk_util_sleep(1);
    }
}

// Mira Kernel Keyboard Scan Code Set 1 Map
// Maps scancodes to key names for the standard US keyboard layout.
static const char* mk_kbd_scancode_set1_map[0x59] = {
    /* 0x00 */ NULL, "ESC", "1", "2", "3", "4", "5", "6",
    /* 0x08 */ "7", "8", "9", "0", "-", "=", "BACKSPACE", "TAB",
    /* 0x10 */ "Q", "W", "E", "R", "T", "Y", "U", "I",
    /* 0x18 */ "O", "P", "[", "]", "ENTER", "LCTRL", "A", "S",
    /* 0x20 */ "D", "F", "G", "H", "J", "K", "L", ";",
    /* 0x28 */ "'", "`", "LSHIFT", "\\", "Z", "X", "C", "V",
    /* 0x30 */ "B", "N", "M", ",", ".", "/", "RSHIFT", "KP*",
    /* 0x38 */ "LALT", "SPACE", "CAPSLOCK", "F1", "F2", "F3", "F4", "F5",
    /* 0x40 */ "F6", "F7", "F8", "F9", "F10", "NUMLOCK", "SCROLLLOCK", "KP7",
    /* 0x48 */ "KP8", "KP9", "KP-", "KP4", "KP5", "KP6", "KP+", "KP1",
    /* 0x50 */ "KP2", "KP3", "KP0", "KP.", NULL, NULL, NULL, "F11",
    /* 0x58 */ "F12"
};

// Mira Kernel Keyboard Extended Code Active
// State variable for handling E0 prefixed scancodes.
static uint8_t mk_kbd_extended_code_active = 0;

// Mira Kernel Keyboard Get Key
// Polls the keyboard controller for a scancode and attempts to map it.
const char* mk_keyboard_get_key() {
    uint8_t status;
    uint8_t scancode;
    const char* key_name = NULL;

    status = mk_util_inb(MK_KBD_STATUS_PORT);

    if (status & MK_KBD_STAT_OBF) { // Data is available
        scancode = mk_util_inb(MK_KBD_DATA_PORT);

        if (mk_kbd_extended_code_active) {
            // E0 means extended key (keys not on the main part of the keyboard).
            // We need to turn E0 keys into a name or NULL if not mapped.
            if (!(scancode & 0x80)) { // 0x80 = make code (key press)
                switch (scancode) {
                    case 0x48: key_name = "UP_ARROW"; break;
                    case 0x50: key_name = "DOWN_ARROW"; break;
                    case 0x4B: key_name = "LEFT_ARROW"; break;
                    case 0x4D: key_name = "RIGHT_ARROW"; break;
                    case 0x1D: key_name = "RCTRL"; break;
                    case 0x38: key_name = "RALT"; break; 
                    case 0x52: key_name = "INSERT"; break;
                    case 0x53: key_name = "DELETE"; break;
                    case 0x47: key_name = "HOME"; break;
                    case 0x4F: key_name = "END"; break;
                    case 0x49: key_name = "PAGE_UP"; break;
                    case 0x51: key_name = "PAGE_DOWN"; break;
                    case 0x5B: key_name = "LGUI"; break;
                    case 0x5C: key_name = "RGUI"; break;
                    case 0x5D: key_name = "APPS"; break;
                    case 0x35: key_name = "KP/"; break;
                    case 0x1C: key_name = "KP_ENTER"; break;
                    // TODO: Add more extended keys
                }
            }
            mk_kbd_extended_code_active = 0; // Consume the E0 prefix state.
            return key_name; // Return name or NULL if not mapped/release.
        }

        if (scancode == 0xE0) {
            mk_kbd_extended_code_active = 1; // Set state for next byte.
            return NULL; // No key pressed yet.
        }
        
        // Process non-extended scancodes.
        if (!(scancode & 0x80)) { // It's a make code (key press).
            if (scancode < (sizeof(mk_kbd_scancode_set1_map) / sizeof(mk_kbd_scancode_set1_map[0]))) {
                key_name = mk_kbd_scancode_set1_map[scancode];
            } else {
                key_name = "UNKNOWN"; // Scancode out of our map's range.
            }
        } else {
            key_name = NULL;
        }
        return key_name;
    }

    return NULL; // No key pressed or no data available.
}