#include "include/uconfy.h"

char *uconfig_device_id;
char *uconfig_api_key;
void (*uconfig_success_callback)();
int uconfig_success_callback_called = 0;

void uconfig_start(char *device_id, char *api_key, void (*callback)(), int poll_interval_seconds) {
    uconfig_device_id = device_id;
    uconfig_api_key = api_key;
    uconfig_success_callback = callback;
    // uconfig_poll_interval_seconds = poll_interval_seconds;

    uconfig_tmp_log = malloc(sizeof(char) * UCONFIG_TMP_LOG_MAX_SIZE);
    uconfig_tmp_log[0] = '\0';
    // xTaskCreate(uconfig_poller, "config_poll", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
}

/*
void uconfig_poller( void * pvParameters )
{
  for( ;; )
  {
    if (UCONFIG_IS_WIFI_CONNECTED) {
        uconfy_fetch_configs();
    }

    vTaskDelay((uconfig_poll_interval_seconds * 1000) / portTICK_RATE_MS);
  }
}*/
