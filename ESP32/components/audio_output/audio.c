#include "audio.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "driver/i2s_std.h"
#include "esp_log.h"
#include "esp_littlefs.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define AUDIO_BUFFER    2048
    
static const char *TAG = "AUDIO";
static bool timed_out = false;

i2s_chan_handle_t tx_handle;
esp_timer_handle_t timer_handle;

void timer_callback(void *args) 
{
    timed_out = true;
}

esp_err_t audio_init(void)
{
    // Setup standard config and channel
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    ESP_ERROR_CHECK(i2s_new_channel(&chan_cfg, &tx_handle, NULL));

    // Setup i2s config
    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(44100),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = GPIO_NUM_41,
            .ws = GPIO_NUM_42,
            .dout = GPIO_NUM_40,
            .din = I2S_GPIO_UNUSED,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };

    // Init littlefs for audio file
    esp_vfs_littlefs_conf_t config = {
        .base_path = "/littlefs",
        .partition_label = "littlefs",
        .format_if_mount_failed = true,
        .dont_mount = false,
    };
    esp_err_t ret = esp_vfs_littlefs_register(&config);
    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
        {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        }
        else if (ret == ESP_ERR_NOT_FOUND)
        {
            ESP_LOGE(TAG, "Failed to find LittleFS partition");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to initialize LittleFS (%s)", esp_err_to_name(ret));
        }
        return ESP_FAIL;
    }

    return i2s_channel_init_std_mode(tx_handle, &std_cfg);
}

esp_err_t play_wav_file(char *volume_selection, uint8_t test)
{
    FILE *fh = fopen("/littlefs/DallasStars.wav", "rb");
    if (fh == NULL)
    {
    ESP_LOGE(TAG, "Failed to open file");
    return ESP_ERR_INVALID_ARG;
    }

    // skip the header...
    fseek(fh, 44, SEEK_SET);

    // create a writer buffer
    int16_t *buf = calloc(AUDIO_BUFFER, sizeof(int16_t));
    size_t bytes_read = 0;
    size_t bytes_written = 0;
    float signed_sample;
    float volume;

    // Set volume level
    if (strcmp(volume_selection, "Off") == 0) {
        printf("Volume: off\n");
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        return ESP_OK;
    }
    else if (strcmp(volume_selection, "Low") == 0) {
        volume = 0.000001;
        printf("Volume: low\n");
    }
    else if (strcmp(volume_selection, "High") == 0) {
        volume = 1;
        printf("Volume: high\n");
    }
    else {
        volume = 0.5;
        ESP_LOGE(TAG, "No Volume Level Found");
    }

    bytes_read = fread(buf, sizeof(int16_t), AUDIO_BUFFER, fh);

    i2s_channel_enable(tx_handle);

    if (test == 1) {
        const esp_timer_create_args_t esp_timer_create_args = {
            .callback = timer_callback,
            .name = "Speaker Timer"
        };
        esp_timer_create(&esp_timer_create_args, &timer_handle);
        esp_timer_start_once(timer_handle, 2000000);
    }

    while (bytes_read > 0)
    {
        // Adjust audio file based on volume setting
        for (int i = 0; i < bytes_read; i += 2)
        {
            signed_sample = *((int16_t *)(buf + i));
            signed_sample = signed_sample * volume;
            *((int16_t *)(buf + i)) = signed_sample;
        }

        // write the buffer to the i2s
        i2s_channel_write(tx_handle, buf, bytes_read * sizeof(int16_t), &bytes_written, portMAX_DELAY);
        bytes_read = fread(buf, sizeof(int16_t), AUDIO_BUFFER, fh);

        if ((test == 1) && timed_out) {
            esp_timer_stop(timer_handle);
            esp_timer_delete(timer_handle);
            timed_out = false;
            break;
        }
    }

    i2s_channel_disable(tx_handle);
    free(buf);

    return ESP_OK;
}