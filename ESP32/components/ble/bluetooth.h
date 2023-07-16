#ifndef bluetooth_h
#define bluetooth_h

#include "esp_err.h"
#include "../nvs_storage/nvs_storage.h"

esp_err_t all_values_set(void);
void run_ble(User_Info *passed_structure);
void stop_ble(void);

#endif