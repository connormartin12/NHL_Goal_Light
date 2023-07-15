#ifndef bluetooth_h
#define bluetooth_h

#include "esp_err.h"
#include "../nvs_storage/nvs_storage.h"

esp_err_t all_values_set(void);
struct user_info_struct get_user_info(void);
void reset_struct(void);
void run_ble(void);
void stop_ble(void);

#endif