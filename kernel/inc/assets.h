#ifndef MK_ASSETS_H
#define MK_ASSETS_H

#include <stdint.h>
#include <stddef.h>

#define MK_ASSETS_MAGIC 0x6D667330 // "mfs0" assets magic

extern uint8_t assets_start[];
extern uint8_t assets_end[];

void mk_assets_init(void);
const char *mk_get_asset(const char *name, uint32_t *out_size);

#endif