#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
// #include "freertos/event_groups.h"
#include "nvs.h"
#include "nvs_flash.h"

#include "bluetooth.h"
#include "ota.h"
#include "wifi.h"

#define NVS_TAG "NVS"
#define OTA_TAG "OTA"
#define WIFI_TAG "WiFi"

const char infoKey[] = "key";
User_Info userInfo;  
size_t userInfoSize = sizeof(User_Info);
nvs_handle user_info_handle;

void request_user_info()
{
    run_ble();
    esp_err_t err = all_values_set();
    if (err)
        ESP_ERROR_CHECK(all_values_set());
    stop_ble();
    userInfo = get_user_info();
    reset_struct();
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init_partition("MyNvs"));
    ESP_ERROR_CHECK(nvs_open_from_partition("MyNvs", "info_store", NVS_READWRITE, &user_info_handle));

    esp_err_t result = nvs_get_blob(user_info_handle, infoKey, (void *)&userInfo, &userInfoSize);
    switch (result)
    {
        case ESP_ERR_NVS_NOT_FOUND:
            ESP_LOGE(NVS_TAG, "User info not set yet");
            request_user_info();
            break;
        case ESP_OK:
            ESP_LOGI(NVS_TAG, "SSID: %s, Password: %s", userInfo.wifi_ssid, userInfo.wifi_password);
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

    wifi_connect_sta(userInfo.wifi_ssid, userInfo.wifi_password);

    esp_err_t err = run_ota();
    if (err) 
        ESP_LOGE(OTA_TAG, "Failed to perform OTA upadate");

    /* Temp nvs storage code */
    // if (result != ESP_OK)
    // {
    //     sprintf(userInfo.wifi_ssid, "AubbyWiFi");
    //     sprintf(userInfo.wifi_password, "Cinnamon1234");
    //     ESP_ERROR_CHECK(nvs_set_blob(user_info_handle, infoKey, (void*)&userInfo, userInfoSize));
    //     ESP_ERROR_CHECK(nvs_commit(user_info_handle));
    //     nvs_close(user_info_handle);
    // }
}