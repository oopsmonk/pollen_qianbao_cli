#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "settings.h"

static void settings_read_file(char path[], wallet_setting_t *setting) {
  FILE *conf = NULL;
  errno = 0;
  conf = fopen(path, "r");
  if (errno != 0) {
    printf("Open configure file failed %s\n", strerror(errno));
    exit(1);
  }
  fread(setting->seed, TANGLE_SEED_BYTES, 1, conf);
  fread(&setting->last_addr, sizeof(uint64_t), 1, conf);
  fread(&setting->first_unspent_idx, sizeof(uint64_t), 1, conf);
  fread(&setting->last_unspent_idx, sizeof(uint64_t), 1, conf);
  fclose(conf);
}

// remove trailing newline from user input
static void strip(char buf[]) {
  char *pos = NULL;
  if ((pos = strchr(buf, '\n')) != NULL) {
    *pos = '\0';
  }
}

wallet_setting_t *wallet_settings_read(char path[]) {
  char read_buf[128] = {};
  wallet_setting_t *c = malloc(sizeof(wallet_setting_t));
  bool new_seed = false;

  if (c) {
    printf("Enter the node endpoint: ");
    fgets(read_buf, sizeof(read_buf), stdin);
    if (strlen(read_buf) > 1) {
      strip(read_buf);
      c->url = malloc(strlen(read_buf));
      if (c->url == NULL) {
        wallet_setting_free(c);
        printf("[%s:%d] memory allocation failed\n", __func__, __LINE__);
        exit(-1);
      }
      strncpy(c->url, read_buf, strlen(read_buf));
    } else {
      printf("please enter a node endpoint");
      exit(-1);
    }

    // memset(read_buf, 0, sizeof(read_buf));
    printf("Enter port number (empty for default): ");
    fgets(read_buf, sizeof(read_buf), stdin);
    if (strlen(read_buf) > 1) {
      c->port = atoi(read_buf);
    }

    if (path == NULL) {
      // memset(read_buf, 0, sizeof(read_buf));
      printf("Enter seed (Enter for a random seed): ");
      fgets(read_buf, sizeof(read_buf), stdin);
      if (strlen(read_buf) > 1) {
        strip(read_buf);
        seed_from_base58(read_buf, c->seed);
      } else {
        printf("Wallet will genrate a seed for you\n");
        new_seed = true;
        random_seed(c->seed);
      }

      if (!new_seed) {
        // memset(read_buf, 0, sizeof(read_buf));
        printf("Enter last address index: ");
        fgets(read_buf, sizeof(read_buf), stdin);
        c->last_addr = strtoll(read_buf, NULL, 10);

        // memset(read_buf, 0, sizeof(read_buf));
        printf("Enter first unspent index: ");
        fgets(read_buf, sizeof(read_buf), stdin);
        c->first_unspent_idx = strtoll(read_buf, NULL, 10);

        // memset(read_buf, 0, sizeof(read_buf));
        printf("Enter last unspent index: ");
        fgets(read_buf, sizeof(read_buf), stdin);
        c->last_unspent_idx = strtoll(read_buf, NULL, 10);
      }

    } else {
      // read config from file
      settings_read_file(path, c);
    }
  }
  return c;
}

void wallet_setting_free(wallet_setting_t *setting) {
  if (setting) {
    if (setting->url) {
      free(setting->url);
    }
    free(setting);
  }
}

void wallet_settings_write(wallet_am_t *am, char path[]) {
  FILE *conf = NULL;
  errno = 0;
  conf = fopen(path, "w+");
  if (errno != 0) {
    printf("write configure file failed %s\n", strerror(errno));
    exit(1);
  }
  fwrite(am->seed, TANGLE_SEED_BYTES, 1, conf);
  fwrite(&am->last_addr_index, sizeof(uint64_t), 1, conf);
  fwrite(&am->first_unspent_idx, sizeof(uint64_t), 1, conf);
  fwrite(&am->last_unspent_idx, sizeof(uint64_t), 1, conf);
  fclose(conf);
}