#include "include/uconfy.h"

char *uconfig_device_id;
char *uconfig_api_key;
void (*commands_fetched_callback)() = NULL;

void uconfig_poller(void *pvParameters)
{
  int uconfig_poll_interval_seconds = 30;
  for( ;; )
  {
    if (UCONFIG_IS_WIFI_CONNECTED) {
        if (commands_fetched_callback != NULL) {
            printf("Poll commands ...\n");
            uconfy_fetch_commands();
        } else {
            printf("Avoid polling commands, there is no callback defined ...\n");
        }
    }

    vTaskDelay((uconfig_poll_interval_seconds * 1000) / portTICK_RATE_MS);
  }
}


void uconfy_init(char *device_id, char *api_key, void (*commands_fetched)()) {
    uconfig_device_id = device_id;
    uconfig_api_key = api_key;
    commands_fetched_callback = *commands_fetched;

    uconfig_tmp_log[0] = '\0';
    if (commands_fetched != NULL) {
        xTaskCreate(uconfig_poller, "config_poll", 4096, NULL, 1, NULL);
    }
}
