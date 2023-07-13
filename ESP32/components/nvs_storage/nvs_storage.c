#include <stdio.h>
#include "nvs.h"

#define TAG "NVS"
static const char infoKey[] = "key";

typedef struct user_info
{
    char wifi_ssid[33]; // 32 characters is the max ssid length per standards
    char wifi_password[64]; // 63 character is the max password length per standards
} User_Info;

char *get_user_info()
{
    
}

esp_err_t storage_init()
{
    ESP_ERROR_CHECK(nvs_flash_init_partition("MyNvs"));
    nvs_handle user_info_handle;
    ESP_ERROR_CHECK(nvs_open_from_partition("MyNvs", "info_store", NVS_READ, &user_info_handle));

    User_Info info;
    size_t userInfoSize = sizeof(User_Info);

    esp_err_t result = nvs_get_blob(user_info_handle, infoKey, (void *)&info, *userInfoSize);
    if (result == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGE(TAG, "Value not set yet");
        return result;
    }
    else if (result == ESP_OK) {
        ESP_LOGI(TAG, "SSID: %s, Password: %s", info.ssid, info.password);
        return ESP_OK;
    }
    else {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(result));
        return ESP_FAIL;
    }
}