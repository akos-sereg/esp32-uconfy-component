#include "include/uconfy.h"

struct uconfig_item *uconfig_repo;
struct uconfig_command_item *uconfig_command_repo;
int uconfig_repo_len = 0;
int uconfig_command_repo_len = 0;

void uconfig_parse_http_response() {
    if (uconfig_repo == NULL) {
        uconfig_repo = malloc(sizeof(struct uconfig_item) * UCONFIG_REPO_MAXITEMS);
    }

    char line[64];
    int char_index = 0;
    int header_over = 0;
    uconfig_repo_len = 0;
    for (int i=0; i!=strlen(response_payload); i++) {

        if (i >= 3 && response_payload[i] == '\n' && response_payload[i-1] == '\r'
            && response_payload[i-2] == '\n' && response_payload[i-3] == '\r') {
            header_over = 1;
        }

        if (!header_over) {
            continue;
        }

        if (response_payload[i] == '\r') {
            continue;
        }

        if (response_payload[i] == '\n') {
            line[char_index] = '\0';
            if (strlen(line) > 0) {
                uconfig_register_config_item(line);
                uconfig_repo_len++;
            }

            char_index = 0;
            continue;
        }

        line[char_index] = response_payload[i];
        char_index++;
    }
}

void uconfig_parse_http_response_for_commands() {
    char line[64];
    int char_index = 0;
    int header_over = 0;
    uconfig_command_repo_len = 0;
     if (uconfig_command_repo == NULL) {
        uconfig_command_repo = malloc(sizeof(struct uconfig_command_item) * UCONFIG_REPO_MAXITEMS);
    }
    for (int i=0; i!=strlen(response_payload_commands); i++) {

        if (i >= 3 && response_payload_commands[i] == '\n' && response_payload_commands[i-1] == '\r'
            && response_payload_commands[i-2] == '\n' && response_payload_commands[i-3] == '\r') {
            header_over = 1;
        }

        if (!header_over) {
            continue;
        }

        if (response_payload_commands[i] == '\r') {
            continue;
        }

        if (response_payload_commands[i] == '\n') {
            line[char_index] = '\0';
            if (strlen(line) > 0) {
                uconfig_register_command_item(line);
                uconfig_command_repo_len++;
            }

            char_index = 0;
            continue;
        }

        line[char_index] = response_payload_commands[i];
        char_index++;
    }
}

void uconfig_register_config_item(char *line) {
    int separator_pos = -1;

    for (int i=0; i!=strlen(line); i++) {
        if (separator_pos == -1) {
            if (line[i] == '=') {
                separator_pos = i;
                uconfig_repo[uconfig_repo_len].key[i] = '\0';
            } else {
                uconfig_repo[uconfig_repo_len].key[i] = line[i];
            }
        } else {
            uconfig_repo[uconfig_repo_len].value[i-separator_pos-1] = line[i];
        }
    }

    uconfig_repo[uconfig_repo_len].value[strlen(line)-1-separator_pos] = '\0';
    ESP_LOGI(TAG_UCONFIG, "key=[%s], value=[%s]", uconfig_repo[uconfig_repo_len].key, uconfig_repo[uconfig_repo_len].value);

    // store in NVS storage
    set_nvs_value(uconfig_repo[uconfig_repo_len].key, uconfig_repo[uconfig_repo_len].value);
}

void uconfig_register_command_item(char *line) {
    int id_pos = -1;
    int separator_pos = -1;

    printf("--> registering %s\n", line);
    int char_index = 0;

    for (int i=0; i!=strlen(line); i++) {
        printf("-> processing char %c\n", line[i]);
        if (id_pos == -1) {
            printf("-> ID pos not found yet\n");
            if (line[i] == ',') {
                id_pos = i;
            }
        } else {
            printf("-> ID pos found already\n");
            if (separator_pos == -1) {
                printf("-> separator pos not found yet (%d)\n", uconfig_command_repo_len);
                if (line[i] == ',') {
                    separator_pos = i;
                    uconfig_command_repo[uconfig_command_repo_len].command[char_index] = '\0';
                    char_index = 0;
                } else {
                    uconfig_command_repo[uconfig_command_repo_len].command[char_index] = line[i];
                    char_index++;
                }
            } else {
                printf("-> separator pos found already\n");
                uconfig_command_repo[uconfig_repo_len].parameter[char_index] = line[i];
                char_index++;
            }
        }
    }

    uconfig_command_repo[uconfig_command_repo_len].parameter[char_index] = '\0';
    ESP_LOGI(TAG_UCONFIG, "command=[%s], parameter=[%s]", uconfig_command_repo[uconfig_command_repo_len].command, uconfig_command_repo[uconfig_command_repo_len].parameter);
}

char *uconfy_get_string_param(char *key, char *fallback) {
    for (int i=0; i!=uconfig_repo_len; i++) {
        if (strncmp(key, uconfig_repo[i].key, strlen(key)) == 0 && strlen(key) == strlen(uconfig_repo[i].key)) {
            return uconfig_repo[i].value;
        }
    }

    return fallback;
}

int uconfy_get_int_param(char *key, int fallback) {

    for (int i=0; i!=uconfig_repo_len; i++) {
        if (strncmp(key, uconfig_repo[i].key, strlen(key)) == 0 && strlen(key) == strlen(uconfig_repo[i].key)) {
            return atoi(uconfig_repo[i].value);
        }
    }

    return fallback;
}