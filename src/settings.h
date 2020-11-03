#pragma once

#include <stdbool.h>

#include "core/types.h"
#include "wallet/address_manager.h"

typedef struct {
  byte_t seed[TANGLE_SEED_BYTES];
  uint16_t port;
  uint64_t last_addr;
  uint64_t first_unspent_idx;
  uint64_t last_unspent_idx;
  char *url;
} wallet_setting_t;

wallet_setting_t *wallet_settings_read(char path[]);

void wallet_setting_free(wallet_setting_t *setting);

void wallet_settings_write(wallet_am_t *am, char path[]);
