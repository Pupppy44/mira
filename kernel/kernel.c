// sound_sb16.c
// Minimal example of Sound Blaster 16 initialization and simple WAV playback
// using a bare-bones environment (no standard library).

#include <stdint.h>
#include "../assets/theme.h"

// Common SB16 register addresses (example 0x220 base). Adjust if your base differs.
#define SB16_BASE       0x220
#define SB16_RESET      (SB16_BASE + 0x06)  // Usually 0x226
#define SB16_READ       (SB16_BASE + 0x0A)  // Usually 0x22A
#define SB16_WRITE      (SB16_BASE + 0x0C)  // Usually 0x22C
#define SB16_READ_STATUS (SB16_BASE + 0x0E) // Usually 0x22E (8-bit IRQ acknowledge)
#define SB16_MIXER_ADDR (SB16_BASE + 0x04)  // Usually 0x224
#define SB16_MIXER_DATA (SB16_BASE + 0x05)  // Usually 0x225

// DMA registers (for 8-bit channel 1)
#define DMA1_CHAN_MASK      0x0A
#define DMA1_MODE           0x0B
#define DMA1_CLEAR_FF       0x0C
#define DMA1_ADDR_CH1       0x02
#define DMA1_COUNT_CH1      0x03
#define DMA1_PAGE_CH1       0x83

// Example I/O helper for your OS (replace with your actual in/out routines).
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "dN"(port));
}
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "dN"(port));
    return ret;
}

//--------------------------------------
// DSP Communication
//--------------------------------------
static int sb16_reset_dsp(void) {
    // Reset the SB16 DSP by writing 1 then 0 to reset port
    outb(SB16_RESET, 1);
    // Delay at least 3µs (bare-metal: do a short wait or run ~ few instructions)
    for (volatile int i = 0; i < 1000; i++) { /* short delay */ }
    outb(SB16_RESET, 0);

    // DSP should respond with 0xAA on the read port if it’s alive
    for (int retry = 0; retry < 1000; retry++) {
        if (inb(SB16_READ) == 0xAA) {
            return 0; // success
        }
    }
    return -1; // failed to detect
}

static void sb16_write_dsp(uint8_t val) {
    // Poll until DSP can accept a command
    // The write port status <bit7> = 1 means DSP is busy
    for (int retry = 0; retry < 10000; retry++) {
        if (!(inb(SB16_BASE + 0x0C) & 0x80)) { 
            outb(SB16_WRITE, val);
            return;
        }
    }
}

static uint8_t sb16_read_dsp(void) {
    // For reading DSP data or version
    return inb(SB16_READ);
}

//--------------------------------------
// Mixer Helpers
//--------------------------------------
static void sb16_mixer_write(uint8_t reg, uint8_t val) {
    outb(SB16_MIXER_ADDR, reg);
    outb(SB16_MIXER_DATA, val);
}
static uint8_t sb16_mixer_read(uint8_t reg) {
    outb(SB16_MIXER_ADDR, reg);
    return inb(SB16_MIXER_DATA);
}

//--------------------------------------
// DMA Setup (8-bit, single mode example)
//--------------------------------------
static void sb16_program_dma_8bit(uint32_t addr, uint16_t length) {
    // length is the total bytes minus 1 for DSP command
    // e.g., if you want to play 0x1000 bytes, pass 0x1000 - 1 = 0x0FFF to DSP.

    // 1) Disable channel 1
    outb(DMA1_CHAN_MASK, 0x05);  // (channel number + 4) => 1+4=5
    // 2) Clear flip-flop
    outb(DMA1_CLEAR_FF, 0x00);

    // 3) Single-cycle or auto-init mode
    //    0x48 = single mode + read transfer + channel
    //    0x58 = auto-init + read transfer + channel
    //    For channel=1 => 0x48 or 0x58
    outb(DMA1_MODE, 0x58 | 1); // example: single mode, channel 1

    // 4) Send page
    uint8_t page = (addr >> 16) & 0xFF;
    outb(DMA1_PAGE_CH1, page);

    // 5) Send offset (low/high)
    uint16_t offset = addr & 0xFFFF;
    outb(DMA1_ADDR_CH1, (uint8_t)(offset & 0xFF));      // low
    outb(DMA1_ADDR_CH1, (uint8_t)((offset >> 8) & 0xFF));// high

    // 6) Send length (low/high)
    //    length is # of bytes minus 1
    outb(DMA1_COUNT_CH1, (uint8_t)(length & 0xFF));      
    outb(DMA1_COUNT_CH1, (uint8_t)((length >> 8) & 0xFF));

    // 7) Enable channel 1
    outb(DMA1_CHAN_MASK, 1);
}

//--------------------------------------
// Minimal function to play 8-bit WAV data from wav_data[]
//--------------------------------------
void mk_snd_play_sound(void) {
    // 1) Reset the DSP to confirm SB16 presence
    if (sb16_reset_dsp() != 0) {
        // Could not find SB16, handle error or return
        return;
    }

    // 2) Set master volume (Mixer: 0x22). 
    //    Each nibble is left/right volume (0x0F = max). Usually you can do 0xFF for both channels.
    sb16_mixer_write(0x22, 0xFF); // max volume L=0xF, R=0xF

    // 3) Turn speaker on (Digital output)
    sb16_write_dsp(0xD1);
    uint32_t data_len = 20 * 1024; // Max size of the Sound Blaster 16 DMA buffer
    // 4) Program DMA for 8-bit playback
    //    The DSP command requires length-1 in the 16-bit field, so for data_len bytes,
    //    we pass (data_len - 1) to the DSP.
    //    You also need the physical address for “wav_data.” In a real OS, ensure it’s physical, not virtual.
    sb16_program_dma_8bit((uint32_t)(uintptr_t)theme, data_len - 1);

    // 5) Set sample rate using command 0x41 (Set Output Sample Rate)
    //    For example 11025 Hz or 22050 Hz, etc.
    //    The command wants high byte, then low byte.
    uint16_t sample_rate = 8000;
    sb16_write_dsp(0x41);                       // set output rate
    sb16_write_dsp((sample_rate >> 8) & 0xFF);  // rate high
    sb16_write_dsp(sample_rate & 0xFF);         // rate low

    // 6) Send the “8-bit DMA mode” command (0xC0 for 8-bit playback) 
    //    bits 3..0 => 0 => speaker on, FIFO off, ...
    sb16_write_dsp(0xC0); // 8-bit DMA mode

    // 7) Send Audio Format (Mono/Unsigned) => e.g. 0x00 => bits: stereo=0, signed=0, etc.
    sb16_write_dsp(0x00); // This means 8-bit mono unsigned

    // 8) Send the playback length - 1 (low byte, high byte) 
    //    data_len - 1 (matching what we did for the DMA)
    uint16_t play_length = (uint16_t)(data_len - 1);
        sb16_write_dsp((play_length >> 8) & 0xFF);   // high
    sb16_write_dsp((play_length & 0xFF));        // low

    // The card should now start playing the PCM data in auto-init or single-cycle mode.
    // If you used single mode, the playback stops automatically after playing data_len bytes.
    // If you used auto-init, you’d need to handle interrupts and keep feeding data.

    // 10) Wait or do other tasks… Once playback ends or you want to stop:
    // Example “stop 8-bit channel” => 0xD0. 
    // sb16_write_dsp(0xD0);
}

void mk_entry() {
    // Play the sound
    mk_snd_play_sound();

    // Hang forever
    while (1) { }
}