#include <stdio.h>
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "ota.h"
#include "wifi.h"

#define NVS_TAG "NVS"
#define OTA_TAG "OTA"
#define WIFI_TAG "WiFi"

typedef struct user_info_struct
{
    char wifi_ssid[33];
    char wifi_password[64];
} User_Info;

/* Temporary semaphore code */
SemaphoreHandle_t ota_semaphore;

void ota_func(void *params)
{
    while (true)
    {
        xSemaphoreTake(ota_semaphore, portMAX_DELAY);
        esp_err_t err = run_ota();
        if (err) 
            ESP_LOGE(OTA_TAG, "Failed to perform OTA upadate");
    }
}

void on_button_pushed(void *params)
{
    xSemaphoreGiveFromISR(ota_semaphore, pdFALSE);
}
/* Temporary semaphore code ends */

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init_partition("MyNvs"));
    nvs_handle user_info_handle;
    ESP_ERROR_CHECK(nvs_open_from_partition("MyNvs", "info_store", NVS_READWRITE, &user_info_handle));

    char infoKey[] = "key";
    User_Info userInfo;
    size_t userInfoSize = sizeof(User_Info);

    esp_err_t result = nvs_get_blob(user_info_handle, infoKey, (void *)&userInfo, &userInfoSize);
    switch (result)
    {
        case ESP_ERR_NVS_NOT_FOUND:
            ESP_LOGE(NVS_TAG, "Value not set yet");
            break;
        case ESP_OK:
            ESP_LOGI(NVS_TAG, "SSID: %s, Password: %s", userInfo.wifi_ssid, userInfo.wifi_password);
            break;
        default:
            ESP_LOGE(NVS_TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(result));
            break;
    }

    /* Temp nvs storage code */
    // if (result != ESP_OK)
    // {
    //     sprintf(userInfo.wifi_ssid, "AubbyWiFi");
    //     sprintf(userInfo.wifi_password, "Cinnamon1234");
    //     ESP_ERROR_CHECK(nvs_set_blob(user_info_handle, infoKey, (void*)&userInfo, userInfoSize));
    //     ESP_ERROR_CHECK(nvs_commit(user_info_handle));
    //     nvs_close(user_info_handle);
    // }

    /* Normal flash init */
    esp_err_t retry = nvs_flash_init();
    if (retry == ESP_ERR_NVS_NO_FREE_PAGES || ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        retry = nvs_flash_init();
    }
    ESP_ERROR_CHECK(retry);

    wifi_connect_sta("AubbyWiFi", "Cinnamon1234");

    /* Temporary button/semaphore code */
    gpio_config_t gpioConfig = {
        .pin_bit_mask = 1ULL << GPIO_NUM_0,
        .mode = GPIO_MODE_DEF_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLUP_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE,
    };
    gpio_config(&gpioConfig);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(GPIO_NUM_0, on_button_pushed, NULL);

    ota_semaphore = xSemaphoreCreateBinary();
    xTaskCreate(ota_func, "run_ota", 1024 * 8, NULL, 2, NULL);
    /* Temporary button/semaphore code ends */
}