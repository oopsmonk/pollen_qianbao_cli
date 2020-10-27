#include <inttypes.h>
#include <stdio.h>

#include "wallet/wallet.h"

char const *const endpoint = "https://api.goshimmer/";

int main(int argc, char **argv) {
  byte_t seed[TANGLE_SEED_BYTES];
  seed_from_base58("332Db2RL4NHggDX4utnn5sCwTVTqUQJ3vC42TGZFC8hK", seed);
  wallet_t *w = wallet_init(endpoint, 0, seed, 7, 6, 7);
  wallet_status_print(w);

  bool synced = wallet_is_node_synced(w);
  printf("Is endpoint synced? %s\n", synced ? "Yes" : "No");
  printf("balance: %" PRIu64 "\n", wallet_balance(w));
  wallet_free(w);
  return 0;
}
