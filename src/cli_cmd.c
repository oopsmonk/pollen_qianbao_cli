#include <ctype.h>
#include <inttypes.h>
#include <stdio.h>
#include <time.h>

#include "argtable3.h"
#include "cli_cmd.h"
#include "settings.h"
#include "split_argv.h"
#include "uthash.h"

#include "client/api/get_node_info.h"
#include "wallet/wallet.h"

#define CLI_LINE_BUFFER 4096
#define CLI_MAX_ARGC 16

typedef struct {
  wallet_t *wallet;    /*!< Pollen wallet instance */
  char *parsing_buf;   /*!< buffer for command line parsing */
  UT_array *cmd_array; /*!< an array of registed commands */
} cli_ctx_t;

static cli_ctx_t cli_ctx;

static void cmd_icd_copy(void *_dst, const void *_src) {
  cli_cmd_t *dst = (cli_cmd_t *)_dst, *src = (cli_cmd_t *)_src;
  dst->command = src->command ? strdup(src->command) : NULL;
  dst->help = src->help ? strdup(src->help) : NULL;
  dst->hint = src->hint ? strdup(src->hint) : NULL;
  dst->func = src->func;
  dst->argtable = src->argtable;
}

static void cmd_icd_dtor(void *_elt) {
  cli_cmd_t *elt = (cli_cmd_t *)_elt;
  if (elt->command) {
    free(elt->command);
  }
  if (elt->help) {
    free(elt->help);
  }
  if (elt->hint) {
    free(elt->hint);
  }
}

UT_icd cli_cmd_icd = {sizeof(cli_cmd_t), NULL, cmd_icd_copy, cmd_icd_dtor};

void completion_callback(char const *buf, linenoiseCompletions *lc) {
  size_t len = strlen(buf);
  cli_cmd_t *cmd_p = NULL;
  if (len == 0) {
    return;
  }

  while ((cmd_p = (cli_cmd_t *)utarray_next(cli_ctx.cmd_array, cmd_p))) {
    if (strncmp(buf, cmd_p->command, len) == 0) {
      linenoiseAddCompletion(lc, cmd_p->command);
    }
  }
}

char *hints_callback(char const *buf, int *color, int *bold) {
  size_t len = strlen(buf);
  cli_cmd_t *cmd_p = NULL;
  if (len == 0) {
    return NULL;
  }

  while ((cmd_p = (cli_cmd_t *)utarray_next(cli_ctx.cmd_array, cmd_p))) {
    if (strlen(cmd_p->command) == len && strncmp(buf, cmd_p->command, len) == 0) {
      *color = HINT_COLOR_GREEN;
      *bold = 0;
      return cmd_p->hint;
    }
  }
  return NULL;
}

void to_uppercase(char *sPtr, int nchar) {
  for (int i = 0; i < nchar; i++) {
    char *element = sPtr + i;
    *element = toupper((unsigned char)*element);
  }
}

//==========COMMANDS==========

static cli_err_t fn_help(int argc, char **argv) {
  UNUSED(argc);
  UNUSED(argv);
  cli_cmd_t *cmd_p = NULL;

  while ((cmd_p = (cli_cmd_t *)utarray_next(cli_ctx.cmd_array, cmd_p))) {
    char const *help = (cmd_p->help) ? cmd_p->help : "";
    if (cmd_p->hint != NULL) {
      printf("- %s %s\n", cmd_p->command, cmd_p->hint);
      printf("    %s\n", cmd_p->help);
    } else {
      printf("- %s %s\n", cmd_p->command, help);
    }
    if (cmd_p->argtable) {
      arg_print_glossary(stdout, (void **)cmd_p->argtable, "  %12s  %s\n");
    }
    printf("\n");
  }
  return CLI_OK;
}

static void cmd_register_help() {
  cli_cmd_t cmd = {
      .command = "help",
      .help = "Show this help",
      .hint = NULL,
      .func = &fn_help,
      .argtable = NULL,
  };

  utarray_push_back(cli_ctx.cmd_array, &cmd);
}

//========== Exit ==========
static cli_err_t fn_exit(int argc, char **argv) {
  UNUSED(argc);
  UNUSED(argv);
  wallet_settings_write(cli_ctx.wallet->addr_manager, "./wallet.bin");
  printf("Settings write to ./wallet.bin\n");
  return CLI_OK;
}

static void cmd_register_exit() {
  cli_cmd_t cmd = {
      .command = "exit",
      .help = "Exist and Save wallet status",
      .hint = NULL,
      .func = &fn_exit,
      .argtable = NULL,
  };
  utarray_push_back(cli_ctx.cmd_array, &cmd);
}

//==========NODE_INFO==========
static cli_err_t fn_node_info(int argc, char **argv) {
  UNUSED(argc);
  UNUSED(argv);

  res_node_info_t info;
  int ret = get_node_info(&cli_ctx.wallet->endpoint, &info);
  if (ret == 0) {
    printf("Node: %s is %s\n", cli_ctx.wallet->endpoint.url, info.is_synced ? "synced" : "unsynced");
    printf("ID: %s Version: %s\n", info.id, info.version);
  } else {
    printf("Error: get node info failed\n");
  }

  return CLI_OK;
}

static void cmd_register_node_info() {
  cli_cmd_t cmd = {
      .command = "node_info",
      .help = "Shows node info",
      .hint = NULL,
      .func = &fn_node_info,
      .argtable = NULL,
  };
  utarray_push_back(cli_ctx.cmd_array, &cmd);
}

//========== Balance ==========
static cli_err_t fn_balance(int argc, char **argv) {
  UNUSED(argc);
  UNUSED(argv);
  printf("Wallet balance: %" PRIu64 "\n", wallet_balance(cli_ctx.wallet));

  return CLI_OK;
}

static void cmd_register_balance() {
  cli_cmd_t cmd = {
      .command = "balance",
      .help = "Shows wallet balance",
      .hint = NULL,
      .func = &fn_balance,
      .argtable = NULL,
  };
  utarray_push_back(cli_ctx.cmd_array, &cmd);
}

//========== Addresses ==========
static cli_err_t fn_addresses(int argc, char **argv) {
  char addr_str[TANGLE_ADDRESS_BASE58_BUF];
  addr_list_t *addrs = am_addresses(cli_ctx.wallet->addr_manager);
  address_t *elm = NULL;
  ADDR_LIST_FOREACH(addrs, elm) {
    address_2_base58(elm->addr, addr_str);
    printf("[%" PRIu64 "] %s %s\n", elm->index, addr_str,
           am_is_spent_address(cli_ctx.wallet->addr_manager, elm->index) ? "[spent]" : "");
  }

  return CLI_OK;
}

static void cmd_register_addresses() {
  cli_cmd_t cmd = {
      .command = "addresses",
      .help = "list addresses",
      .hint = NULL,
      .func = &fn_addresses,
      .argtable = NULL,
  };
  utarray_push_back(cli_ctx.cmd_array, &cmd);
}

//========== Wallet Info ==========
static cli_err_t fn_wallet_info(int argc, char **argv) {
  wallet_refresh(cli_ctx.wallet, true);
  wallet_status_print(cli_ctx.wallet);
  return CLI_OK;
}

static void cmd_register_wallet_info() {
  cli_cmd_t cmd = {
      .command = "wallet_info",
      .help = "Shows wallet information",
      .hint = NULL,
      .func = &fn_wallet_info,
      .argtable = NULL,
  };
  utarray_push_back(cli_ctx.cmd_array, &cmd);
}

//========== Request Funds ==========
static cli_err_t fn_req_funds(int argc, char **argv) {
  wallet_request_funds(cli_ctx.wallet);
  return CLI_OK;
}

static void cmd_register_req_funds() {
  cli_cmd_t cmd = {
      .command = "req_funds",
      .help = "Request funds from the facet",
      .hint = NULL,
      .func = &fn_req_funds,
      .argtable = NULL,
  };
  utarray_push_back(cli_ctx.cmd_array, &cmd);
}

//========== SEED ==========
static int fn_seed(int argc, char **argv) {
  char seed_str[TANGLE_SEED_BASE58_BUF];
  if (seed_2_base58(cli_ctx.wallet->addr_manager->seed, seed_str) == true) {
    printf("%s\n", seed_str);
  } else {
    printf("Gets seed failed\n");
  }
  return 0;
}

static void cmd_register_seed() {
  cli_cmd_t cmd = {
      .command = "seed",
      .help = "Shows the seed of this wallet",
      .hint = NULL,
      .func = &fn_seed,
      .argtable = NULL,
  };
  utarray_push_back(cli_ctx.cmd_array, &cmd);
}

//========== New Address ==========
static int fn_new_address(int argc, char **argv) {
  byte_t addr[TANGLE_ADDRESS_BYTES];
  char addr_str[TANGLE_ADDRESS_BASE58_BUF];
  am_get_new_address(cli_ctx.wallet->addr_manager, addr);
  if (address_2_base58(addr, addr_str)) {
    printf("%s\n", addr_str);
  } else {
    printf("Converting address to base58 failed\n");
  }
  return 0;
}

static void cmd_register_new_address() {
  cli_cmd_t cmd = {
      .command = "new_address",
      .help = "Gest a new address",
      .hint = NULL,
      .func = &fn_new_address,
      .argtable = NULL,
  };
  utarray_push_back(cli_ctx.cmd_array, &cmd);
}

//========== Send Funds ==========
static struct {
  struct arg_str *receiver;
  struct arg_str *value;
  struct arg_end *end;
} send_funds_args;

static int fn_send_funds(int argc, char **argv) {
  int nerrors = arg_parse(argc, argv, (void **)&send_funds_args);
  if (nerrors != 0) {
    arg_print_errors(stderr, send_funds_args.end, argv[0]);
    return -1;
  }

  // check parameters
  char const *receiver = send_funds_args.receiver->sval[0];
  char *endptr = NULL;
  int64_t value = strtoll(send_funds_args.value->sval[0], &endptr, 10);
  if (value <= 0) {
    printf("Error: funds <= 0\n");
    return -1;
  }

  send_funds_op_t send_op = {};
  send_op.amount = value;
  if (!address_from_base58(receiver, send_op.receiver)) {
    printf("Invalid receiver address\n");
    return -1;
  }
  return wallet_send_funds(cli_ctx.wallet, &send_op);
}

static void cmd_register_send_funds() {
  send_funds_args.receiver = arg_str1(NULL, NULL, "<RECEIVER>", "A receiver address in base58 encoding");
  send_funds_args.value = arg_str0("v", "value", "<VALUE>", "A token value");
  send_funds_args.end = arg_end(2);

  cli_cmd_t cmd = {
      .command = "send_funds",
      .help = "Issue a payment.",
      .hint = NULL,
      .func = &fn_send_funds,
      .argtable = &send_funds_args,
  };
  utarray_push_back(cli_ctx.cmd_array, &cmd);
}

//==========END OF COMMANDS==========

cli_err_t cli_command_init(char url[], uint16_t port, byte_t seed[], uint64_t last_addr_idx, uint64_t first_unspent_idx,
                           uint64_t last_unspent_idx) {
  // init command line paring buffer
  cli_ctx.parsing_buf = calloc(CLI_LINE_BUFFER, 1);
  if (cli_ctx.parsing_buf == NULL) {
    printf("command line buffer allocating failed\n");
    return CLI_ERR_OOM;
  }

  cli_ctx.wallet = wallet_init(url, port, seed, last_addr_idx, first_unspent_idx, last_unspent_idx);
  if (cli_ctx.wallet == NULL) {
    printf("wallet init failed\n");
    free(cli_ctx.parsing_buf);
    return CLI_ERR_OOM;
  }

  // create cmd list
  utarray_new(cli_ctx.cmd_array, &cli_cmd_icd);
  if (cli_ctx.cmd_array == NULL) {
    printf("command register init failed\n");
    free(cli_ctx.parsing_buf);
    wallet_free(cli_ctx.wallet);
    return CLI_ERR_OOM;
  }

  // registing commands
  cmd_register_help();
  cmd_register_exit();
  cmd_register_node_info();
  cmd_register_balance();
  cmd_register_addresses();
  cmd_register_wallet_info();
  cmd_register_req_funds();
  cmd_register_new_address();
  cmd_register_seed();
  cmd_register_send_funds();

  return CLI_OK;
}

cli_err_t cli_command_cleanup() {
  wallet_free(cli_ctx.wallet);
  free(cli_ctx.parsing_buf);
  utarray_free(cli_ctx.cmd_array);
  return CLI_OK;
}

cli_err_t cli_command_run(char const *const cmdline, cli_err_t *cmd_ret) {
  if (cli_ctx.parsing_buf == NULL) {
    return CLI_ERR_NULL_POINTER;
  }

  char **argv = (char **)calloc(CLI_MAX_ARGC, sizeof(char *));
  if (argv == NULL) {
    return CLI_ERR_OOM;
  }

  strncpy(cli_ctx.parsing_buf, cmdline, CLI_LINE_BUFFER);

  // split command line
  size_t argc = console_split_argv(cli_ctx.parsing_buf, argv, CLI_MAX_ARGC);
  if (argc == 0) {
    free(argv);
    return CLI_ERR_INVALID_ARG;
  }

  // run command
  cli_cmd_t *cmd_p = NULL;
  while ((cmd_p = (cli_cmd_t *)utarray_next(cli_ctx.cmd_array, cmd_p))) {
    if (!strcmp(cmd_p->command, argv[0])) {
      *cmd_ret = (*cmd_p->func)(argc, argv);
    }
  }
  free(argv);
  return CLI_OK;
}
