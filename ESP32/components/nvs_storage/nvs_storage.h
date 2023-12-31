#ifndef nvs_storage_h
#define nvs_storage_h

#include "esp_err.h"

#define PARTITION_NAME "MyNvs"

typedef struct user_info_struct {
    char wifi_ssid[35]; // 32 characters is the maximum length of a wifi ssid
    char wifi_password[65]; // 63 characters is the maximum wifi password length
    char team[70]; // Maximum NHL team length is 54 characters adding 16 characters for safety
    char team_name[40]; // Just team name, no additonal JSON data
    char team_abbrev[5]; // Max abbrev size is 4
    char delay[5]; // Maximum delay value possible would be 3 digits at most
    char volume[7]; // Maximum value would be "medium"
} User_Info;

esp_err_t get_stored_info(User_Info *userSettings);
void store_user_info(User_Info *userSettings);
void erase_user_info(void);

#endif