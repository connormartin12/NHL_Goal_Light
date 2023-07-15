#ifndef nvs_storage_h
#define nvs_storage_h

#include "esp_err.h"

#define PARTITION_NAME "MyNvs"

typedef struct user_info_struct
{
    char wifi_ssid[33];
    char wifi_password[64];
} User_Info;

esp_err_t get_stored_info(User_Info *userSettings);
void store_user_info(User_Info *userSettings);

#endif