#include "../inc/assets.h"
#include "../inc/util.h"

// Mira Kernel Assets Initialization
void mk_assets_init(void) {
    const uint32_t *magic = (const uint32_t *)assets_start;
    if (*magic != MK_ASSETS_MAGIC) {
        for (;;) {
            mk_util_print("mk_assets_init: assets magic mismatch, halting...\n");
            __asm__ volatile("hlt");
        }
    }
}

// Mira Kernel Get Asset
const char *mk_get_asset(const char *name, uint32_t *out_size) {
    const uint8_t *p = assets_start;
    if (*(const uint32_t *)p != MK_ASSETS_MAGIC) {
        return NULL;
    }
    p += 4;

    uint32_t count = *(const uint32_t *)p;
    p += 4;

    const uint8_t *entry = p;
    for (uint32_t i = 0; i < count; i++) {
        uint8_t nlen = *entry++;
        entry += nlen;
        entry += 4;
        entry += 4;
    }

    const uint8_t *payload = entry;
    uint32_t offset = 0;
    entry = p;

    for (uint32_t i = 0; i < count; i++) {
        uint8_t nlen = *entry++;
        const char *fname = (const char *)entry;
        entry += nlen;
        entry += 4;
        uint32_t len = *(const uint32_t *)entry;
        entry += 4;

        uint8_t match = 1;
        for (uint8_t j = 0; j < nlen; j++) {
            if ((uint8_t)name[j] != (uint8_t)fname[j]) {
                match = 0;
                break;
            }
        }

        if (match && name[nlen] == '\0') {
            if (out_size) {
                *out_size = len;
            }
            return (const char *)(payload + offset);
        }

        offset += len;
    }

    return NULL;
}