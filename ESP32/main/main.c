#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#include "bluetooth.h"
#include "nvs_storage.h"
#include "oled.h"
#include "ota.h"
#include "wifi.h"

#define NVS_TAG "NVS"
#define OTA_TAG "OTA"
#define WIFI_TAG "WiFi"

User_Info userInfo;
char *defaultDelay = "30";

void request_user_info()
{
    run_ble(&userInfo);
    esp_err_t err = all_values_set();
    if (err) {
        ESP_ERROR_CHECK(all_values_set());
    }
    stop_ble();
    store_user_info(&userInfo);
}

void app_main(void)
{
    // Initialize OLED display
    initialize_oled();

    // Checks for first time device use. If device has never been used before, it will immediately enter ble mode to request user input.
    esp_err_t result = get_stored_info(&userInfo);
    switch (result)
    {
        case ESP_ERR_NVS_NOT_FOUND:
            ESP_LOGE(NVS_TAG, "User info not set yet");
            memcpy(userInfo.delay, defaultDelay, sizeof(&defaultDelay));
            request_user_info();
            break;
        case ESP_OK:
            ESP_LOGI(NVS_TAG, "User info found");
            break;
        default:
            ESP_LOGE(NVS_TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(result));
            break;
    }

    esp_err_t retry = nvs_flash_init();
    if (retry == ESP_ERR_NVS_NO_FREE_PAGES || ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        retry = nvs_flash_init();
    }
    ESP_ERROR_CHECK(retry);

    wifi_init();

    // If ESP32 cannot connect to the wifi, it should keep returning to ble mode to request correct wifi credentials from the user.
    const char *wifi_connecting_text = ("Connecting to wifi. . .");
    set_oled_text(wifi_connecting_text);
    retry = wifi_connect_sta(userInfo.wifi_ssid, userInfo.wifi_password);
    while (retry != ESP_OK) {
        wifi_disconnect();
        request_user_info();
        set_oled_text(wifi_connecting_text);
        retry = wifi_connect_sta(userInfo.wifi_ssid, userInfo.wifi_password);
    }

    esp_err_t err = run_ota();
    if (err) 
        ESP_LOGE(OTA_TAG, "Failed to perform OTA upadate");
}