#include <stdio.h>
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "nvs_storage.h"
#include "ota.h"
#include "wifi.h"

#define NVS_TAG "NVS"
#define OTA_TAG "OTA"
#define WIFI_TAG "WiFi"

typedef struct wifi_credentials_struct
{
    char ssid[50];
    char password[50];
} WiFi_Credentials;

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
    esp_err_t retry = nvs_flash_init();
    if (retry == ESP_ERR_NVS_NO_FREE_PAGES || ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        retry = nvs_flash_init();
    }
    ESP_ERROR_CHECK(retry);

    // ESP_ERROR_CHECK(nvs_flash_init_partition("MyNvs"));
    // nvs_handle credential_storage;
    // ESP_ERROR_CHECK(nvs_open_from_partition("MyNvs", "cred_store", NVS_READWRITE, &credential_storage));

    // static const char credentialsKey[] = "key";
    // WiFi_Credentials my_credentials;
    // size_t credentialSize = sizeof(WiFi_Credentials);

    // esp_err_t result = nvs_get_blob(credential_storage, credentialsKey, (void *)&my_credentials, &credentialSize);
    esp_err_t result = storage_init();
    switch (result)
    {
        case ESP_ERR_NVS_NOT_FOUND:
            ESP_LOGE(NVS_TAG, "Value not set yet");
            // sprintf(my_credentials.ssid, "AubbyWiFi");
            // sprintf(my_credentials.password, "Cinnamon1234");
            // ESP_ERROR_CHECK(nvs_set_blob(credential_storage, credentialsKey, (void*)&my_credentials, sizeof(WiFi_Credentials)));
            // ESP_ERROR_CHECK(nvs_commit(credential_storage));
            // nvs_close(credential_storage);
            // esp_restart();
            break;
        case ESP_OK:
            ESP_LOGI(NVS_TAG, "SSID: %s, Password: %s", my_credentials.ssid, my_credentials.password);
            break;
        default:
            ESP_LOGE(NVS_TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(result));
            break;
    }

    wifi_init();
    esp_err_t err = wifi_connect_sta("AubbyWiFi", "Cinnamon1234", 10000);
    if (err) ESP_LOGE(WIFI_TAG, "Failed to connect to the AP");

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