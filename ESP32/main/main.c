#include <stdio.h>
#include "esp_log.h"
#include "nvs_flash.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "ota.h"
#include "wifi.h"

#define WIFI_TAG "WiFi"

/* Temporary semaphore code */
SemaphoreHandle_t ota_semaphore;

void ota_func(void *params)
{
    while (true)
    {
        xSemaphoreTake(ota_semaphore, portMAX_DELAY);
        run_ota();
        printf("Hello!!!");
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