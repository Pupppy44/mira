#ifndef MOUSE_H
#define MOUSE_H

#include <stdint.h>
#include <stddef.h>

// PS/2 Controller Ports, same as keyboard but used differently
#define MK_MOUSE_DATA_PORT   0x60
#define MK_MOUSE_STATUS_PORT 0x64
#define MK_MOUSE_CMD_PORT    0x64

// PS/2 Status Port Masks
#define MK_MSTAT_OBF 0x01 // Output Buffer Full
#define MK_MSTAT_IBF 0x02 // Input Buffer Full
#define MK_MSTAT_AUX 0x20 // Data is from auxiliary device (mouse)

// PS/2 Mouse Commands
#define MK_MCMD_SET_SAMPLE_RATE 0xF3
#define MK_MCMD_ENABLE_STREAM   0xF4
#define MK_MCMD_SET_DEFAULTS    0xF6
#define MK_MCMD_RESET           0xFF

// Structure to hold the current state of the mouse
typedef struct {
    int32_t x;
    int32_t y;
    uint8_t left_button;
    uint8_t right_button;
    uint8_t middle_button;
} mk_mouse_state_t;

// Function to initialize the mouse
void mk_mouse_init();

// Function to get the current state of the mouse
// This function polls for new data and updates an internal state.
// It returns a pointer to that internal state.
mk_mouse_state_t* mk_mouse_get_state();

#endif