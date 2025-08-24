#include "../inc/mouse.h"
#include "../inc/util.h"

// Universal polling function declared in keyboard.c
void mk_ps2_poll_devices();

// Internal mouse state variables
static uint8_t mouse_packet_cycle = 0;
static uint8_t mouse_packet_buffer[3];
static mk_mouse_state_t current_mouse_state = {0, 0, 0, 0, 0};

// Mira Kernel Mouse Process Packet
// This function is called by the unified poller whenever a mouse byte is available.
void mk_mouse_process_packet(uint8_t data) {
    switch (mouse_packet_cycle) {
        case 0:
            // Sanity check: Since a PS/2 mouse uses 3-byte packets, 
            // we need to check the packet size to ensure validity
            if (data & 0x08) {
                mouse_packet_buffer[0] = data;
                mouse_packet_cycle++;
            }
            break;
        case 1:
            // Second byte (X movement)
            mouse_packet_buffer[1] = data;
            mouse_packet_cycle++;
            break;
        case 2:
            // Third byte (Y movement), packet complete
            mouse_packet_buffer[2] = data;
            mouse_packet_cycle = 0; // Reset for next packet

            // Process the complete packet
            current_mouse_state.left_button = mouse_packet_buffer[0] & 0x1;
            current_mouse_state.right_button = (mouse_packet_buffer[0] & 0x2) >> 1;
            current_mouse_state.middle_button = (mouse_packet_buffer[0] & 0x4) >> 2;

            int16_t delta_x = mouse_packet_buffer[1];
            int16_t delta_y = mouse_packet_buffer[2];

            // Handle sign bits for X and Y
            if (mouse_packet_buffer[0] & 0x10) {
                delta_x |= 0xFF00; // Sign extend
            }
            if (mouse_packet_buffer[0] & 0x20) {
                delta_y |= 0xFF00; // Sign extend
            }

            current_mouse_state.x += delta_x;
            current_mouse_state.y -= delta_y; // Y is inverted

            // TODO: Clamp mouse coordinates once Mira
            // has graphics support (e.g., 640x400)

            break;
    }
}

// Mira Kernel Mouse Wait For Send Ready
// Waits until the controller's input buffer is empty.
static void mk_mouse_wait_for_send_ready() {
    while (mk_util_inb(MK_MOUSE_STATUS_PORT) & MK_MSTAT_IBF);
}

// Mira Kernel Mouse Wait For Receive Ready
// Waits until the controller's output buffer is full.
static void mk_mouse_wait_for_recv_ready() {
    while (!(mk_util_inb(MK_MOUSE_STATUS_PORT) & MK_MSTAT_OBF));
}

// Mira Kernel Mouse Send Command
// Sends a command byte to the mouse itself.
static void mk_mouse_send_command(uint8_t cmd) {
    mk_mouse_wait_for_send_ready();
    mk_util_outb(MK_MOUSE_CMD_PORT, 0xD4); // Command to send to auxiliary device
    mk_mouse_wait_for_send_ready();
    mk_util_outb(MK_MOUSE_DATA_PORT, cmd);
}

// Mira Kernel Mouse Read Data
// Reads a data byte from the mouse.
static uint8_t mk_mouse_read_data() {
    mk_mouse_wait_for_recv_ready();
    return mk_util_inb(MK_MOUSE_DATA_PORT);
}

// Mira Kernel Mouse Initialize
// Performs basic initialization for the mouse.
void mk_mouse_init() {
    // 1. Enable the auxiliary device (the mouse)
    mk_mouse_wait_for_send_ready();
    mk_util_outb(MK_MOUSE_CMD_PORT, 0xA8);

    // 2. Set mouse defaults
    mk_mouse_send_command(MK_MCMD_SET_DEFAULTS);
    mk_mouse_read_data(); // Acknowledge

    // 3. Enable data reporting (streaming mode)
    mk_mouse_send_command(MK_MCMD_ENABLE_STREAM);
    mk_mouse_read_data(); // Acknowledge
}

// Mira Kernel Mouse Get State
// Polls devices and returns the current state.
mk_mouse_state_t* mk_mouse_get_state() {
    // Call the unified poller to process any pending hardware data first.
    mk_ps2_poll_devices();
    
    // Return a pointer to the internal state, which is updated by the poller.
    return &current_mouse_state;
}