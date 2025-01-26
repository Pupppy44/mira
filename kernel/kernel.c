#include "../assets/desktop.h"
#include "inc/gfx.h"

int mk_entry() {
    mk_gfx_clear_screen(0x02); // Set the screen to green
    mk_gfx_set_pixels(desktop, &(mk_gfx_dimensions_t){0, 0, 320, 200});

    while (1);

    return 0; // success
}