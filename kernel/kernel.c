#include "inc/mira.h"
#include "inc/gfx.h"
#include <stdint.h>

#define VGA_WIDTH 320
#define VGA_HEIGHT 200
#define VGA_ADDRESS 0xA0000

int mk_entry() {
    mk_gfx_set_pixels(mira2, &(mk_gfx_dimensions_t){0, 0, MIRA2_WIDTH, MIRA2_HEIGHT});

    while (1);

    return 0; // success
}