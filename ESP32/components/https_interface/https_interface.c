#include <stdio.h>
#include <string.h>
#include "esp_http_client.h"
#include "esp_log.h"

#include "parse_JSON.h"

static const char *TAG = "HTTPS";
extern const uint8_t cert[] asm("_binary_NHL_API_crt_start");

typedef struct chunk_payload_t
{
    uint8_t *buffer;
    int buffer_index;
} chunk_payload_t;

esp_err_t on_client_data(esp_http_client_event_t *evt)
{
    switch(evt->event_id)
    {
        case HTTP_EVENT_ON_DATA:
        {
            chunk_payload_t *chunk_payload = evt->user_data;
            chunk_payload->buffer = realloc(chunk_payload->buffer, chunk_payload->buffer_index + evt->data_len + 1); // +1 is for null terminator
            memcpy(&chunk_payload->buffer[chunk_payload->buffer_index], (uint8_t *)evt->data, evt->data_len);
            chunk_payload->buffer_index = chunk_payload->buffer_index + evt->data_len;
            chunk_payload->buffer[chunk_payload->buffer_index] = NULL;

            // The following print statement WAS causing a WDT error (watchdogtimer)
            // printf("buffer******** %s\n", chunk_payload->buffer);
        }   
            break;
        default:
            break; 
    }
    return ESP_OK;
}

void https_test()
{
    chunk_payload_t chunk_payload = {0};

    esp_http_client_config_t esp_http_client_config = {
        .url = "https://statsapi.web.nhl.com/api/v1/teams/25",
        .method = HTTP_METHOD_GET,
        .event_handler = on_client_data,
        .user_data = &chunk_payload,
        .cert_pem = (char *)cert,
    };
    esp_http_client_handle_t client = esp_http_client_init(&esp_http_client_config);
    esp_err_t err = esp_http_client_perform(client);
    if(err == ESP_OK)
    {
        ESP_LOGI(TAG, "GET status = %d", esp_http_client_get_status_code(client));
        printf("buffer******** %s\n", chunk_payload.buffer);

        parse_buffer((char *)chunk_payload.buffer);
    }
    else
    {
        ESP_LOGE(TAG, "GET request failed: %s", esp_err_to_name(err));
    }
    if(chunk_payload.buffer != NULL)
    {
        free(chunk_payload.buffer);
    }

    esp_http_client_cleanup(client);
}