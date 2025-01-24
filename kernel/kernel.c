#include "inc/mira.h"
#include "inc/gfx.h"
#include <stdint.h>

    volatile uint8_t* video = (uint8_t*)0xA0000;


int mk_entry() {
    mk_gfx_map_colors();
    mk_gfx_draw_bitmap(&mo, 128, 128, 0, 0);

    while (1);

    return 0; // success
}
