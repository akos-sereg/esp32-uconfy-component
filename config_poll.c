#include "include/config_poll.h"

#define WEB_SERVER "192.168.1.10"
#define WEB_PORT "8080"

char *uconfig_device_id;
char *uconfig_api_key;
char response_payload[1024];
char request[7500]; /* max length of logs is 5000 chars, so 7500 should be enough if we base64 encode it */
void (*uconfig_success_callback)();
int uconfig_success_callback_called = 0;
int uconfig_poll_interval_seconds = 60;

static const char *OK_RESPONSE = "HTTP/1.1 200 OK";
static const char *NO_CONTENT_RESPONSE = "HTTP/1.1 204 No Content";
static const char *REQUEST = "Host: "WEB_SERVER":"WEB_PORT"\r\n"
    "User-Agent: esp-idf/1.0 esp32\r\n";

void uconfig_poller( void * pvParameters )
{
  for( ;; )
  {
    if (UCONFIG_IS_WIFI_CONNECTED) {
        uconfy_fetch_configs();
    }

    vTaskDelay((uconfig_poll_interval_seconds * 1000) / portTICK_RATE_MS);
  }
}

void uconfy_flush_logs()
{
    if (strlen(uconfig_tmp_log) == 0) {
        ESP_LOGI(TAG_UCONFIG, "No logs to flush");
        return;
    }

    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };
    struct addrinfo *res;
    struct in_addr *addr;
    int s, r;
    char recv_buf[64];

    int err = getaddrinfo(WEB_SERVER, WEB_PORT, &hints, &res);

    if(err != 0 || res == NULL) {
        ESP_LOGE(TAG_UCONFIG, "DNS lookup failed err=%d res=%p", err, res);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        return;
    }

    /* Code to print the resolved IP.
    Note: inet_ntoa is non-reentrant, look at ipaddr_ntoa_r for "real" code */
    addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
    ESP_LOGI(TAG_UCONFIG, "DNS lookup succeeded. IP=%s", inet_ntoa(*addr));

    s = socket(res->ai_family, res->ai_socktype, 0);
    if(s < 0) {
        ESP_LOGE(TAG_UCONFIG, "... Failed to allocate socket.");
        freeaddrinfo(res);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        return;
    }
    ESP_LOGI(TAG_UCONFIG, "... allocated socket");

    if(connect(s, res->ai_addr, res->ai_addrlen) != 0) {
        ESP_LOGE(TAG_UCONFIG, "... socket connect failed errno=%d", errno);
        close(s);
        freeaddrinfo(res);
        vTaskDelay(4000 / portTICK_PERIOD_MS);
        return;
    }

    ESP_LOGI(TAG_UCONFIG, "... connected");
    freeaddrinfo(res);

    size_t out_len;
    printf("[base64 encoding] message: '%s'\n", uconfig_tmp_log);
    char *base64_encoded = base64_encode(uconfig_tmp_log, strlen(uconfig_tmp_log), &out_len);
    if (base64_encoded == NULL) {
        ESP_LOGE(TAG_UCONFIG, "Unable to base64 encode payload, memory is full; left space from heap: %d bytes, log site: %d bytes", xPortGetFreeHeapSize(), strlen(uconfig_tmp_log));
        close(s);
        return;
    }

    /*strncat(content, base64_encoded, out_len);
    strcat(content, "\"}");
    strcat(content, "\r\n\0");*/

    sprintf(request, "POST /api/device/%s/logs?api_key=%s HTTP/1.0\r\n%sContent-Type: application/json\r\nContent-Length: %d\r\n\r\n{ \"Logs\": \"%s\" }\r\n",
        uconfig_device_id,
        uconfig_api_key,
        REQUEST,
        strlen(base64_encoded) + 14,
        base64_encoded);

    printf("----------------------\n");
    printf(request);
    printf("\n");
    printf("----------------------\n");

    if (write(s, request, strlen(request)) < 0) {
        ESP_LOGE(TAG_UCONFIG, "... socket send failed");
        close(s);
        free(base64_encoded);
        vTaskDelay(4000 / portTICK_PERIOD_MS);
        return;
    }
    ESP_LOGI(TAG_UCONFIG, "... socket send success");
    uconfig_tmp_log[0] = '\0';
    free(base64_encoded);

    struct timeval receiving_timeout;
    receiving_timeout.tv_sec = 5;
    receiving_timeout.tv_usec = 0;
    if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout,
            sizeof(receiving_timeout)) < 0) {
        ESP_LOGE(TAG_UCONFIG, "... failed to set socket receiving timeout");
        close(s);
        vTaskDelay(4000 / portTICK_PERIOD_MS);
        return;
    }
    ESP_LOGI(TAG_UCONFIG, "... set socket receiving timeout success");

    /* Read HTTP response */
    // response_payload[0] = '\0';
    char response_header[500];
    int content_length = 0;
    do {
        bzero(recv_buf, sizeof(recv_buf));
        r = read(s, recv_buf, sizeof(recv_buf)-1);
        for(int i = 0; i < r; i++) {

            if (content_length < 500) {
                response_header[content_length] = recv_buf[i];
                content_length++;
            }
        }
    } while(r > 0);
    response_header[content_length] = '\0';

    ESP_LOGI(TAG_UCONFIG, "... done reading from socket. Last read return=%d errno=%d.", r, errno);
    close(s);

    if (strncmp(response_header, NO_CONTENT_RESPONSE, strlen(NO_CONTENT_RESPONSE)) == 0) {
        ESP_LOGI(TAG_UCONFIG, "Logs flushed to uconfy server");
    } else {
        ESP_LOGE(TAG_UCONFIG, "Logs could not be flushed to uconfy server, response status is not '204 No Content', full response:");
        ESP_LOGE(TAG_UCONFIG, "%s", response_header);
    }
}

void uconfy_fetch_configs() {
    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };
    struct addrinfo *res;
    struct in_addr *addr;
    int s, r;
    char recv_buf[64];

    int err = getaddrinfo(WEB_SERVER, WEB_PORT, &hints, &res);

    if(err != 0 || res == NULL) {
        ESP_LOGE(TAG_UCONFIG, "DNS lookup failed err=%d res=%p", err, res);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        return;
    }

    addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
    ESP_LOGI(TAG_UCONFIG, "DNS lookup succeeded. IP=%s", inet_ntoa(*addr));

    s = socket(res->ai_family, res->ai_socktype, 0);
    if(s < 0) {
        ESP_LOGE(TAG_UCONFIG, "... Failed to allocate socket.");
        freeaddrinfo(res);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        return;
    }
    ESP_LOGI(TAG_UCONFIG, "... allocated socket");

    if(connect(s, res->ai_addr, res->ai_addrlen) != 0) {
        ESP_LOGE(TAG_UCONFIG, "... socket connect failed errno=%d", errno);
        close(s);
        freeaddrinfo(res);
        vTaskDelay(4000 / portTICK_PERIOD_MS);
        return;
    }

    ESP_LOGI(TAG_UCONFIG, "... connected");
    freeaddrinfo(res);

    char *request = malloc(sizeof(char) * (100 + strlen(REQUEST) + strlen(uconfig_device_id) + strlen(uconfig_api_key)));

    sprintf(request, "GET /api/device/%s/config?api_key=%s HTTP/1.0\r\n%sContent-Type: text/plain\r\n\r\n",
        uconfig_device_id,
        uconfig_api_key,
        REQUEST);

    printf("----------------------\n");
    printf(request);
    printf("\n");
    printf("----------------------\n");

    if (write(s, request, strlen(request)) < 0) {
        ESP_LOGE(TAG_UCONFIG, "... socket send failed");
        close(s);
        free(request);
        vTaskDelay(4000 / portTICK_PERIOD_MS);
        return;
    }
    ESP_LOGI(TAG_UCONFIG, "... socket send success");
    memset(uconfig_tmp_log, 0, sizeof(char) * UCONFIG_TMP_LOG_MAX_SIZE);
    free(request);

    struct timeval receiving_timeout;
    receiving_timeout.tv_sec = 5;
    receiving_timeout.tv_usec = 0;
    if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout,
            sizeof(receiving_timeout)) < 0) {
        ESP_LOGE(TAG_UCONFIG, "... failed to set socket receiving timeout");
        close(s);
        vTaskDelay(4000 / portTICK_PERIOD_MS);
        return;
    }
    ESP_LOGI(TAG_UCONFIG, "... set socket receiving timeout success");

    response_payload[0] = '\0';
    int content_length = 0;
    do {
        bzero(recv_buf, sizeof(recv_buf));
        r = read(s, recv_buf, sizeof(recv_buf)-1);
        for(int i = 0; i < r; i++) {

            if (content_length < 500) {
                response_payload[content_length] = recv_buf[i];
                content_length++;
            }
        }
    } while(r > 0);

    ESP_LOGI(TAG_UCONFIG, "... done reading from socket. Last read return=%d errno=%d.", r, errno);
    close(s);

    if (strncmp(response_payload, OK_RESPONSE, strlen(OK_RESPONSE)) == 0) {
        uconfig_parse_http_response();
        ESP_LOGI(TAG_UCONFIG, "Configs fetched from uconfy server");
    } else {
        ESP_LOGE(TAG_UCONFIG, "Configs could not be fetched from uconfy server, expected 200 OK response. Received full payload:");
        ESP_LOGE(TAG_UCONFIG, "%s", response_payload);
    }
}

void uconfig_start(char *device_id, char *api_key, void (*callback)(), int poll_interval_seconds) {
    uconfig_device_id = device_id;
    uconfig_api_key = api_key;
    uconfig_success_callback = callback;
    uconfig_poll_interval_seconds = poll_interval_seconds;

    uconfig_tmp_log = malloc(sizeof(char) * UCONFIG_TMP_LOG_MAX_SIZE);
    uconfig_tmp_log[0] = '\0';
    // xTaskCreate(uconfig_poller, "config_poll", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
}