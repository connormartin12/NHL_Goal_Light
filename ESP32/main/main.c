#include <stdio.h>
#include <string.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#include "audio.h"
#include "bluetooth.h"
#include "https_interface.h"
#include "nvs_storage.h"
#include "oled.h"
#include "ota.h"
#include "wifi.h"

#define BLE_TAG     "BLE"
#define NVS_TAG     "NVS"
#define OTA_TAG     "OTA"
#define RTOS_TAG    "RTOS"
#define WIFI_TAG    "WiFi"
#define LED_PIN     38
#define BLE_PIN     39

static TaskHandle_t score_task = NULL;
static TaskHandle_t interrupt_task = NULL;
QueueHandle_t interruptQueue;
esp_err_t retry;

User_Info userInfo;
const char *defaultDelay = "30";
const char *defaultVolume = "Low";
int delay;
bool receiver_waiting = false;

void connect_to_wifi(void)
{
    wifi_disconnect();
    const char *wifi_connecting_text = "Connecting to wifi. . .";
    set_oled_text(wifi_connecting_text);
    retry = wifi_connect_sta(userInfo.wifi_ssid, userInfo.wifi_password);
}

// Anytime the user inputs new info, the wifi will disconnect and reconnect
void request_user_info(void)
{
    retry = ESP_FAIL;
    while (retry != ESP_OK) {
        run_ble(&userInfo);
        esp_err_t check = all_values_set();
        switch (check) {
            case ESP_FAIL:
                ESP_ERROR_CHECK(all_values_set());
                break;
            case ESP_RESET:
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
                delay = atoi(userInfo.delay);
                init_score = true;
                break;
        }
    
        connect_to_wifi();
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
            receiver_waiting = true;
            uint32_t task_finished = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
            if (task_finished == 1) {
                vTaskSuspend(score_task);
                gpio_isr_handler_remove(pinNumber);

                do {
                    vTaskDelay(20 / portTICK_PERIOD_MS);
                } while (gpio_get_level(pinNumber) == 1);
                
                request_user_info();

                gpio_isr_handler_add(pinNumber, gpio_isr_handler, (void *)pinNumber);
                receiver_waiting = false;
                vTaskResume(score_task);
            } else
                ESP_LOGE(RTOS_TAG, "The call to ulTaskNotifyTake timed out");
        }
    }
}

// DO NOT RUN OTHER TASKS WHILE THIS FUNCTION IS RUNNING. RESULTS IN AUDIO GLITCHES
void goal_scored(void)
{
    vTaskDelay((delay * 1000) / portTICK_PERIOD_MS);

    size_t text_size = snprintf(NULL, 0, "%s Score!!!", userInfo.team_name);
    char *buffer = malloc(text_size);
    sprintf(buffer, "%s Score!!!", userInfo.team_name);
    const char *team_scored_text = buffer;

    set_oled_text(team_scored_text);
    gpio_set_level(LED_PIN, 1);
    play_wav_file(userInfo.volume, 0);
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

        if (receiver_waiting == true) {
            xTaskNotifyGive(interrupt_task);
            vTaskDelay(50 / portTICK_PERIOD_MS); // Small delay to avoid starting the task over.
        }
    }
}

void app_main(void)
{
    // Initialize OLED display, audio, and LED pin
    initialize_oled();
    audio_init();
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    // Initialize flash
    retry = nvs_flash_init();
    if (retry == ESP_ERR_NVS_NO_FREE_PAGES || ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        retry = nvs_flash_init();
    }
    ESP_ERROR_CHECK(retry);

    // Initialize wifi
    wifi_init();

    // Checks for first time device use. If device has never been used before, it will immediately enter ble mode to request user input.
    const char *stored_info_text = "Looking for stored settings. . .";
    set_oled_text(stored_info_text);
    esp_err_t result = get_stored_info(&userInfo);
    switch (result)
    {
        case ESP_ERR_NVS_NOT_FOUND:
            ESP_LOGW(NVS_TAG, "User info not set yet");
            memcpy(userInfo.delay, defaultDelay, strlen(defaultDelay));
            memcpy(userInfo.volume, defaultVolume, strlen(defaultVolume));
            request_user_info();
            break;
        case ESP_OK:
            ESP_LOGI(NVS_TAG, "User info found");
            connect_to_wifi();
            if (retry != ESP_OK) 
                request_user_info();
            break;
        default:
            ESP_LOGE(NVS_TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(result));
            break;
    }

    // Check for software updates
    esp_err_t err = run_ota();
    if (err) 
        ESP_LOGE(OTA_TAG, "Failed to perform OTA upadate");
    
    // Interrupt pin setup for user to connect to esp BLE at their will
    gpio_set_direction(BLE_PIN, GPIO_MODE_INPUT);
    gpio_pulldown_en(BLE_PIN);
    gpio_pullup_dis(BLE_PIN);
    gpio_set_intr_type(BLE_PIN, GPIO_INTR_POSEDGE);

    interruptQueue = xQueueCreate(10, sizeof(int));
    xTaskCreate(&button_pushed_task, "Run BLE", 10000, NULL, 1, &interrupt_task);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(BLE_PIN, gpio_isr_handler, (void *)BLE_PIN);

    xTaskCreate(&get_score, "Retrieve Score", 10000, NULL, 1, &score_task);
}