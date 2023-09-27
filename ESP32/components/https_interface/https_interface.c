#include "https_interface.h"
#include <stdio.h>
#include <string.h>
#include "esp_http_client.h"
#include "esp_log.h"

#include "parse_JSON.h"

static const char *TAG = "HTTPS";
extern const uint8_t cert[] asm("_binary_NHL_API_crt_start");
// extern const uint8_t cert[] asm("_binary_collegefootballdata_crt_start");

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
            chunk_payload->buffer[chunk_payload->buffer_index] = '\x0';

            // The following print statement WAS causing a WDT error (watchdogtimer)
            // printf("buffer******** %s\n", chunk_payload->buffer);
        }   
            break;
        default:
            break; 
    }
    return ESP_OK;
}

void https_get(char *user_team_name)
{
    chunk_payload_t chunk_payload = {0};

    esp_http_client_config_t esp_http_client_config = {
        // .url = "https://statsapi.web.nhl.com/api/v1/teams/25",
        .url = "https://statsapi.web.nhl.com/api/v1/schedule",
        // .url = "https://api.collegefootballdata.com/scoreboard?classification=fbs&conference=b12",
        .method = HTTP_METHOD_GET,
        .event_handler = on_client_data,
        .user_data = &chunk_payload,
        .cert_pem = (char *)cert,
    };
    esp_http_client_handle_t client = esp_http_client_init(&esp_http_client_config);
    // esp_http_client_set_header(client, "accept", "application/json");
    // esp_http_client_set_header(client, "Authorization", "bearer ZbU1jGAAeWbmmrT3sFsasaY7lickGsa5Wlf54ziDTV0Mr+MrjiUn23XwBRc+1Ngf");
    esp_err_t err = esp_http_client_perform(client);
    if(err == ESP_OK)
    {
        ESP_LOGI(TAG, "GET status = %d", esp_http_client_get_status_code(client));
        // printf("buffer******** %s\n", chunk_payload.buffer);

        // parse_abbreviation((char *)chunk_payload.buffer);
        // parse_football_score((char *)chunk_payload.buffer);
        parse_score((char *)chunk_payload.buffer, user_team_name);
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