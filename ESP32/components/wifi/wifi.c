#include "wifi.h"
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

static int retry_connect = 0;

esp_netif_t *my_netif;

static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (retry_connect < 5) {
            esp_wifi_connect();
            retry_connect++;
            ESP_LOGI(TAG, "Retry to connect to the AP");
        } else {
            retry_connect = 0;
            xEventGroupSetBits(wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG,"Connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        retry_connect = 0;
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void wifi_init(void) {
    ESP_ERROR_CHECK(esp_netif_init());

    // Error here. event loop has already been created. Can't complete more than once.
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));
}

esp_err_t wifi_connect_sta(const char *ssid, const char *pass)
{
        wifi_event_group = xEventGroupCreate();

        
        my_netif = esp_netif_create_default_wifi_sta();

        wifi_config_t wifi_config;
        memset(&wifi_config, 0, sizeof(wifi_config_t));

        strncpy((char *)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid)-1);
        strncpy((char *)wifi_config.sta.password, pass, sizeof(wifi_config.sta.password)-1);

        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
        ESP_ERROR_CHECK(esp_wifi_start()); 

        EventBits_t bits = xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY);
        if (bits & WIFI_CONNECTED_BIT) {
            ESP_LOGI(TAG, "Connected to AP SSID: %s, Password: %s", ssid, pass);
            return ESP_OK;
        }
        else if (bits & WIFI_FAIL_BIT) {
            ESP_LOGI(TAG, "Failed to connect to SSID: %s, Password: %s", ssid, pass);
            return ESP_FAIL;
        }
        else {
            ESP_LOGE(TAG, "UNEXPECTED EVENT");
            return ESP_FAIL;
        }
}

void wifi_disconnect(void)
{
    esp_netif_destroy(my_netif);
    ESP_ERROR_CHECK(esp_wifi_disconnect());
    ESP_ERROR_CHECK(esp_wifi_stop());
}