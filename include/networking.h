#ifndef __networking_h_included__
#define __networking_h_included__

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_wpa2.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

// #include "config_poll.h"

extern EventGroupHandle_t wifi_event_group;
extern int UCONFIG_CONNECTED_BIT;

extern int UCONFIG_IS_WIFI_CONNECTED;
extern void uconfig_initialize_wifi(char *initial_wifi_ssid, char *initial_wifi_password);

#endif