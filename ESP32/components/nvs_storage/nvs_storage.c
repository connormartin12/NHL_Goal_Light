#include "nvs_storage.h"
#include "nvs.h"
#include "nvs_flash.h"

#define PARTITION_NAME "MyNvs"

const char infoKey[] = "key";
size_t userInfoSize = sizeof(User_Info);
nvs_handle user_info_handle;

esp_err_t get_stored_info(User_Info *userSettings)
{
    ESP_ERROR_CHECK(nvs_flash_init_partition(PARTITION_NAME));
    ESP_ERROR_CHECK(nvs_open_from_partition(PARTITION_NAME, "info_store", NVS_READWRITE, &user_info_handle));

    esp_err_t err = nvs_get_blob(user_info_handle, infoKey, (void *)userSettings, &userInfoSize);
    return err;
}

void store_user_info(User_Info *userSettings)
{
    ESP_ERROR_CHECK(nvs_set_blob(user_info_handle, infoKey, (void *)userSettings, userInfoSize));
    ESP_ERROR_CHECK(nvs_commit(user_info_handle));
}