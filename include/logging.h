#ifndef __logging_h_included__
#define __logging_h_included__

#include <string.h>
#include "nvs.h"

#define UCONFIG_TMP_LOG_MAX_SIZE    5000

extern char *uconfig_tmp_log;
extern void uconfig_append_log(char *message);

#endif