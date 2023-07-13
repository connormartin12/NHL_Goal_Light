#include <stdio.h>
#include "nvs.h"

#include "esp_err.h"

esp_err_t storage_init()
{
    printf("Initializing storage!!!\n");
    return ESP_OK;
}