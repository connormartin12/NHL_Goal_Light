#include <stdio.h>
#include "nvs_flash.h"

#include "wifi.h"

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
    ESP_ERROR_CHECK(wifi_connect_sta("AubbyWiFi", "Cinnamon1234", 10000));
}
