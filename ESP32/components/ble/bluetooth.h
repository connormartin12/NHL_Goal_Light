#ifndef bluetooth_h
#define bluetooth_h

#include "esp_err.h"

typedef struct user_info_struct
{
    char wifi_ssid[33];
    char wifi_password[64];
} User_Info;

esp_err_t all_values_set(void);
struct user_info_struct get_user_info(void);
void reset_struct(void);
void run_ble(void);
void stop_ble(void);

#endif