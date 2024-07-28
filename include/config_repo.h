#ifndef __config_repo_h_included__
#define __config_repo_h_included__

#include <stdio.h>
#include <stdlib.h>
#include "fetch_configs.h"
#include "logging.h"
#include "nvs.h"

#define UCONFIG_REPO_MAXITEMS 100

struct uconfig_item
{
    char key[16];
    char value[32];
};

struct uconfig_command_item
{
    char command[32];
    char parameter[32];
};

extern struct uconfig_item *uconfig_repo;
extern int uconfig_repo_len;

extern struct uconfig_command_item *uconfig_command_repo;
extern int uconfig_command_repo_len;

extern char *uconfy_get_string_param(char *key, char *fallback);
extern int uconfy_get_int_param(char *key, int fallback);

extern void uconfig_parse_http_response();
extern void uconfig_parse_http_response_for_commands();
extern void uconfig_register_config_item(char *line);
extern void uconfig_register_command_item(char *line);

#endif