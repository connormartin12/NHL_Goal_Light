#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

static EventGroupHandle_t wifi_event_group;

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

#define TAG "WIFI"

static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch (event_id)
    {
        case WIFI_EVENT_STA_START:
            ESP_LOGI(TAG, "Connecting...");
            ESP_ERROR_CHECK(esp_wifi_connect());
            break;
        case WIFI_EVENT_STA_DISCONNECTED:
            ESP_LOGI(TAG, "Failed to connect to the AP");
            xEventGroupSetBits(wifi_event_group, WIFI_FAIL_BIT);
            break;
        case IP_EVENT_STA_GOT_IP:
            ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
            ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
            xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
            break;
    }
}

void wifi_init(void)
{
    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));
}

esp_err_t wifi_connect_sta(const char *ssid, const char *pass, int timeout)
{
    wifi_event_group = xEventGroupCreate();

    esp_netif_create_default_wifi_sta();

    wifi_config_t wifi_config;
    memset(&wifi_config, 0, sizeof(wifi_config_t));

    strncpy((char *)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid)-1);
    strncpy((char *)wifi_config.sta.password, pass, sizeof(wifi_config.sta.password)-1);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start()); 

    EventBits_t bits = xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE, pdMS_TO_TICKS(timeout));
    if(bits & WIFI_CONNECTED_BIT)
    {
        ESP_LOGI(TAG, "Connected to AP SSID: %s, Password: %s", ssid, pass);
        return ESP_OK;
    }
    ESP_LOGI(TAG, "Failed to connect to SSID: %s, Password: %s", ssid, pass);
    return ESP_FAIL;
}

void wifi_disconnect(void)
{
    ESP_ERROR_CHECK(esp_wifi_disconnect());
    ESP_ERROR_CHECK(esp_wifi_stop());
}