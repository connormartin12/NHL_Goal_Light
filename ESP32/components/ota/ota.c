#include <stdio.h>
#include <string.h>
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "esp_ota_ops.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "../oled_display/oled.h"

#define TAG "OTA"

extern const uint8_t server_cert_pem_start[] asm("_binary_google_crt_start");

esp_err_t client_event_handler(esp_http_client_event_t *evt)
{
    return ESP_OK;
}

esp_err_t validate_image_header(esp_app_desc_t *incoming_ota_desc)
{
    const esp_partition_t *running_partition = esp_ota_get_running_partition();
    esp_app_desc_t running_partition_description;
    esp_ota_get_partition_description(running_partition, &running_partition_description);

    ESP_LOGI(TAG, "Current version is %s", running_partition_description.version);
    ESP_LOGI(TAG, "New version is %s", incoming_ota_desc->version);

    if (strcmp(running_partition_description.version, incoming_ota_desc->version) == 0)
    {
        ESP_LOGW(TAG, "NEW VERSION IS THE SAME AS CURRENT VERSION. ABORTING");
        return ESP_FAIL;
    }

    return ESP_FAIL;
}

esp_err_t run_ota(void)
{
    ESP_LOGI(TAG, "Invoking OTA");
    const char *ota_update_text = "Checking for updates. . .";
    set_oled_text(ota_update_text);

    const esp_http_client_config_t clientConfig = {
        .url = "https://drive.google.com/uc?export=download&id=1K8wCYV8sNd0d8XkOgQblxe5VN6Nqygi3",
        .event_handler = client_event_handler,
        .cert_pem = (char *)server_cert_pem_start,
    };

    esp_https_ota_config_t ota_config = {
        .http_config = &clientConfig,
    };

    esp_https_ota_handle_t ota_handle = NULL;

    if (esp_https_ota_begin(&ota_config, &ota_handle) != ESP_OK)
    {
        ESP_LOGE(TAG, "esp_https_ota_begin_failed");
        return ESP_FAIL;
    }

    esp_app_desc_t incoming_ota_desc;
    if (esp_https_ota_get_img_desc(ota_handle, &incoming_ota_desc) != ESP_OK)
    {
        ESP_LOGE(TAG, "esp_https_ota_get_img_desc failed");
        esp_https_ota_finish(ota_handle);
        return ESP_FAIL;
    }

    if (validate_image_header(&incoming_ota_desc) != ESP_OK)
    {
        const char *no_update_text = "No update available";
        set_oled_text(no_update_text);
        ESP_LOGE(TAG, "validate_image_header failed");
        esp_https_ota_finish(ota_handle);
        return ESP_FAIL;
    }

    const char *ota_update_available_text = "Update Found\n\nInstalling update. . .";
    set_oled_text(ota_update_available_text);
    while (true)
    {
        esp_err_t ota_result = esp_https_ota_perform(ota_handle);
        if (ota_result != ESP_ERR_HTTPS_OTA_IN_PROGRESS)
            break;
    }

    if (esp_https_ota_finish(ota_handle) != ESP_OK)
    {
        ESP_LOGE(TAG, "esp_https_ota_finish failed");
        const char *ota_update_failed_text = "Update failed\n\nPlease try again later";
        set_oled_text(ota_update_failed_text);
        return ESP_FAIL;
    }
    else
    {
        printf("Restarting in 5 seconds\n");
        const char *ota_update_completed_text = "Update complete\n\nRestarting in 5 seconds. . .";
        set_oled_text(ota_update_completed_text);
        vTaskDelay(pdMS_TO_TICKS(5000));
        esp_restart();
    }

}