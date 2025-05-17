#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include <stddef.h>

// Mira uses a PS/2 keyboard controller.
// These are necessary ports for it.
#define MK_KBD_DATA_PORT   0x60
#define MK_KBD_STATUS_PORT 0x64
#define MK_KBD_CMD_PORT    0x64

#define MK_KBD_STAT_OBF 0x01 // Output Buffer Full (OBF)
#define MK_KBD_STAT_IBF 0x02 // Input Buffer Full (IBF)

// Function to initialize the keyboard
void mk_keyboard_init();

// Function to get the name of the last pressed key
const char* mk_keyboard_get_key();

#endif