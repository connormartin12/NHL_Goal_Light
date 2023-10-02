#include <stdio.h>
#include <string.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "nvs_flash.h"

#include "audio.h"
#include "bluetooth.h"
#include "https_interface.h"
#include "nvs_storage.h"
#include "oled.h"
#include "ota.h"
#include "wifi.h"

#define NVS_TAG "NVS"
#define OTA_TAG "OTA"
#define WIFI_TAG "WiFi"
#define BLE_TAG "BLE"
#define LED_PIN 38
#define BLE_PIN 39

static TaskHandle_t score_task = NULL;
QueueHandle_t interruptQueue;

User_Info userInfo;
char *defaultDelay = "30";

void request_user_info()
{
    run_ble(&userInfo);
    esp_err_t err = all_values_set();
    switch (err) {
        case ESP_FAIL:
            ESP_ERROR_CHECK(all_values_set());
            break;
        case ESP_ERR_NOT_FOUND:
            stop_ble();
            erase_user_info();
            const char *reset_text = "Resetting device in 3 seconds. . .";
            set_oled_text(reset_text);
            ESP_LOGW(BLE_TAG, "Restarting in 3 seconds");
            vTaskDelay(3000 / portTICK_PERIOD_MS);
            esp_restart();
            break;
        default:
            stop_ble();
            store_user_info(&userInfo);
            break;
    }
}

static void IRAM_ATTR gpio_isr_handler(void *args)
{
    int pinNumber = (int)args;
    xQueueSendFromISR(interruptQueue, &pinNumber, NULL);
}

void button_pushed_task(void *params)
{
    int pinNumber = 0;
    while (true)
    {
        if (xQueueReceive(interruptQueue, &pinNumber, portMAX_DELAY))
        {
            vTaskSuspend(score_task);
            gpio_isr_handler_remove(pinNumber);

            do
            {
                vTaskDelay(20 / portTICK_PERIOD_MS);
            } while (gpio_get_level(pinNumber) == 1);
            
            request_user_info();

            gpio_isr_handler_add(pinNumber, gpio_isr_handler, (void *)pinNumber);
            vTaskResume(score_task);
        }
    }
}

// DO NOT RUN OTHER TASKS WHILE THIS FUNCTION IS RUNNING. RESULTS IN AUDIO GLITCHES
void goal_scored(void)
{
    // vTaskDelay((userInfo.delay*1000) / portTICK_PERIOD_MS);
    const char *team_scored_text = "Dallas Stars Score!!!";
    set_oled_text(team_scored_text);
    gpio_set_level(LED_PIN, 1);
    play_wav_file();
    gpio_set_level(LED_PIN, 0);
}

void get_score(void *params)
{
    while (true)
    {
        https_get_score(userInfo.team_name);
        if (scored == true)
            goal_scored();
        if (game_found == true) {
            if (init_abbr == true) {
                https_get_abbr(user_team_id);
                https_get_abbr(other_team_id);
                init_abbr = false;
            }
            update_oled_score();

            printf("%s: %d\n%s: %d\n", user_team_abbr, user_team_score, other_team_abbr, other_team_score);
        } else {
            const char *no_game_found_text = ("No game found.");
            set_oled_text(no_game_found_text);
        }

        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    // Initialize OLED display, audio, and LED pin
    initialize_oled();
    audio_init();
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    // Checks for first time device use. If device has never been used before, it will immediately enter ble mode to request user input.
    const char *stored_info_text = ("Looking for stored settings. . .");
    set_oled_text(stored_info_text);
    esp_err_t result = get_stored_info(&userInfo);
    switch (result)
    {
        case ESP_ERR_NVS_NOT_FOUND:
            ESP_LOGW(NVS_TAG, "User info not set yet");
            memcpy(userInfo.delay, defaultDelay, sizeof(&defaultDelay));
            request_user_info();
            break;
        case ESP_OK:
            ESP_LOGI(NVS_TAG, "User info found");
            break;
        default:
            ESP_LOGE(NVS_TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(result));
            break;
    }

    esp_err_t retry = nvs_flash_init();
    if (retry == ESP_ERR_NVS_NO_FREE_PAGES || ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        retry = nvs_flash_init();
    }
    ESP_ERROR_CHECK(retry);

    wifi_init();

    // If ESP32 cannot connect to the wifi, it should keep returning to ble mode to request correct wifi credentials from the user.
    const char *wifi_connecting_text = ("Connecting to wifi. . .");
    set_oled_text(wifi_connecting_text);
    retry = wifi_connect_sta(userInfo.wifi_ssid, userInfo.wifi_password);
    while (retry != ESP_OK) {
        wifi_disconnect();
        request_user_info();
        set_oled_text(wifi_connecting_text);
        retry = wifi_connect_sta(userInfo.wifi_ssid, userInfo.wifi_password);
    }

    // Check for software updates
    esp_err_t err = run_ota();
    if (err) 
        ESP_LOGE(OTA_TAG, "Failed to perform OTA upadate");
    
    /* REMOVE FOLLOWING LINE OF CODE WHEN FINISHED TESTING LIVE GAME*/
    // Calling goal_scored function here for fun
    // goal_scored();

    xTaskCreate(&get_score, "Retrieve Score", 10000, NULL, 1, &score_task);

    // Interrupt pin setup for user to connect to esp BLE at their will
    gpio_set_direction(BLE_PIN, GPIO_MODE_INPUT);
    gpio_pulldown_en(BLE_PIN);
    gpio_pullup_dis(BLE_PIN);
    gpio_set_intr_type(BLE_PIN, GPIO_INTR_POSEDGE);

    interruptQueue = xQueueCreate(10, sizeof(int));
    xTaskCreate(&button_pushed_task, "Run BLE", 10000, NULL, 1, NULL);
    
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BLE_PIN, gpio_isr_handler, (void *)BLE_PIN);
}