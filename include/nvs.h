#ifndef __nvs_h_included__
#define __nvs_h_included__

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"

#include "config_poll.h"

#define NVS_VAL_IN_COMP 0
#define NVS_VAL_IN_GUITAR 1
#define NVS_VAL_IN_VINYL 2

#define NVS_VAL_OUT_HEADPHONES 0
#define NVS_VAL_OUT_SPEAKER 1
#define NVS_VAL_OUT_AMPLIFIER 2

extern void uconfig_load_from_nvs();
extern void init_nvs_store();
extern void set_nvs_value(char *key, char *value);
extern char *get_nvs_value(char *key);
extern void nvs_shutdown();

#include "config_repo.h"

nvs_handle_t app_nvs_handle;
esp_err_t nvs_error;
int nvs_flash_is_open;

char *NVS_KEY_IN_CHANNEL;
char *NVS_KEY_OUT_CHANNEL;
char *NVS_KEY_MUTED;

#endif