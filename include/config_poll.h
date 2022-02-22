#ifndef __config_poll_h_included__
#define __config_poll_h_included__

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "networking.h"
#include "config_repo.h"
#include "nvs.h"
#include "base64.h"

#define TAG_UCONFIG "uconfig"

extern char *uconfig_device_id;
extern char *uconfig_api_key;
extern char response_payload[1024];

extern void (*uconfig_success_callback)();
extern int uconfig_success_callback_called;
extern int uconfig_poll_interval_seconds;

extern void uconfig_poller( void * pvParameters );
extern void uconfy_fetch_configs();
extern void uconfy_flush_logs();
extern void uconfig_start(char *device_id, char *api_key, void (*callback)(), int poll_interval_seconds);

#endif