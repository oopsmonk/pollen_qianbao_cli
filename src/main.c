#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cli_cmd.h"
#include "settings.h"

static uint64_t str2int64(char *str) {
  char *endPtr;
  uint64_t v = strtoll(str, NULL, 10);
  if (*endPtr) {
    return 0;
  }
  return v;
}

int main(int argc, char **argv) {
  UNUSED(argc);
  UNUSED(argv);
  char *line = NULL;

  printf("== Pollen CLI starting....\n");
  wallet_setting_t *config = NULL;
  if (argc >= 2) {
    // read setting from file
    config = wallet_settings_read(argv[1]);
  } else {
    // read setting from stdin
    config = wallet_settings_read(NULL);
  }

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
      // parsing commands
      cli_err_t ret = 0;
      cli_command_run(line, &ret);
      linenoiseHistoryAdd(line); /* Add to the history. */
      if (!strncmp(line, "exit", 4)) {
        break;
      }
    }
    linenoiseFree(line);
  }

  linenoiseFree(line);
  cli_command_cleanup();

  return 0;
}
