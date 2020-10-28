#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cli_cmd.h"

typedef struct {
  char *url;
  char *seed;
  uint16_t port;
  uint64_t last_addr;
  uint64_t first_unspent_idx;
  uint64_t last_unspent_idx;
} wallet_setting_t;

void wallet_setting_free(wallet_setting_t *setting) {
  if (setting) {
    if (setting->url) {
      free(setting->url);
    }
    if (setting->seed) {
      free(setting->seed);
    }
    free(setting);
  }
}

static uint64_t str2int64(char *str) {
  char *endPtr;
  uint64_t v = strtoll(str, NULL, 10);
  if (*endPtr) {
    return 0;
  }
  return v;
}

wallet_setting_t *read_wallet_config() {
  char read_buf[128] = {};
  wallet_setting_t *c = malloc(sizeof(wallet_setting_t));
  bool new_seed = false;

  if (c) {
    printf("Enter the node endpoint: ");
    fgets(read_buf, sizeof(read_buf), stdin);
    if (strlen(read_buf) > 1) {
      c->url = malloc(strlen(read_buf));
      if (c->url == NULL) {
        wallet_setting_free(c);
        printf("[%s:%d] memory allocation failed\n", __func__, __LINE__);
        exit(-1);
      }
      // remove '\n' from fgets
      strncpy(c->url, read_buf, strlen(read_buf) - 1);
      c->url[strlen(read_buf)] = '\0';
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

    // memset(read_buf, 0, sizeof(read_buf));
    printf("Enter seed (empty to genrate a random seed): ");
    fgets(read_buf, sizeof(read_buf), stdin);
    if (strlen(read_buf) > 1) {
      c->seed = malloc(strlen(read_buf));
      if (c->url == NULL) {
        wallet_setting_free(c);
        printf("[%s:%d] memory allocation failed\n", __func__, __LINE__);
        exit(-1);
      }
      // remove '\n' from fgets
      strncpy(c->seed, read_buf, strlen(read_buf) - 1);
      c->seed[strlen(read_buf)] = '\0';
    } else {
      printf("Wallet will genrate a new seed for you\n");
      new_seed = true;
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
  }
  return c;
}

int main(int argc, char **argv) {
  UNUSED(argc);
  UNUSED(argv);
  char *line = NULL;

  printf("== Pollen CLI starting....\n");
  // TODO: read wallet configuration from sotrage
  // printf("== TODO: read wallet settings \n");
  wallet_setting_t *config = read_wallet_config();
  if (config == NULL) {
    printf("read wallet config failed\n");
    exit(-1);
  }

  if (cli_command_init(config->url, config->port, config->seed, config->last_addr, config->first_unspent_idx,
                       config->last_unspent_idx) != CLI_OK) {
    printf("Wallet init failed\n");
    wallet_setting_free(config);
    exit(-1);
  }
  // no needed
  wallet_setting_free(config);

  // Enable multiline mode
  linenoiseSetMultiLine(1);

  /* Set the completion callback. This will be called every time the
   * user uses the <tab> key. */
  linenoiseSetCompletionCallback(completion_callback);
  linenoiseSetHintsCallback(hints_callback);

  /* Now this is the main loop of the typical linenoise-based application.
   * The call to linenoise() will block as long as the user types something
   * and presses enter.
   *
   * The typed string is returned as a malloc() allocated string by
   * linenoise, so the user needs to free() it. */
  while ((line = linenoise("Pollen> ")) != NULL) {
    /* Do something with the string. */
    if (line[0] != '\0' && line[0] != '/') {
      // printf("echo: '%s'\n", line);
      if (!strncmp(line, "exit", 4)) {
        break;
      }
      // parsing commands
      cli_err_t ret = 0;
      cli_command_run(line, &ret);
      linenoiseHistoryAdd(line); /* Add to the history. */
    }
    linenoiseFree(line);
  }

  linenoiseFree(line);
  cli_command_cleanup();

  return 0;
}
