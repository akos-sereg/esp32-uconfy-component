#include "include/logging.h"

char *uconfig_tmp_log;

void uconfig_append_log(char *message) {
    printf("[--> Remote Log <--] %s\n", message);
    if ((strlen(message) + strlen(uconfig_tmp_log) + 1) > UCONFIG_TMP_LOG_MAX_SIZE) {
        ESP_LOGE(TAG_UCONFIG, "Unable to append log, buffer is full.");
        return;
    }

    strcat(uconfig_tmp_log, message);
    strcat(uconfig_tmp_log, "\n");
}