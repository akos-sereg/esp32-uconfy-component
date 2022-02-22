#ifndef __uconfy_h_included__
#define __uconfy_h_included__

#include "flush_logs.h"
#include "fetch_configs.h"
#include "networking.h"
#include "nvs.h"
#include "config_repo.h"

#define TAG_UCONFIG "uconfig"
#define WEB_SERVER "192.168.1.10"
#define WEB_PORT "8080"

static const char *REQUEST = "Host: "WEB_SERVER":"WEB_PORT"\r\n"
    "User-Agent: esp-idf/1.0 esp32\r\n";

extern char *uconfig_device_id;
extern char *uconfig_api_key;

extern void (*uconfig_success_callback)();
extern int uconfig_success_callback_called;
// extern int uconfig_poll_interval_seconds;
extern void uconfig_start(char *device_id, char *api_key, void (*callback)(), int poll_interval_seconds);

// to be internal
// extern void uconfig_poller(void * pvParameters);

#endif