#include "bluetooth.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "../audio_output/audio.h"
#include "cJSON.h"
#include "esp_log.h"
#include "esp_nimble_hci.h"
#include "freertos/event_groups.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "../nvs_storage/nvs_storage.h"
#include "../oled_display/oled.h"
#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#define TAG "BLE"
#define DEVICE_NAME "Goal Light"

#define DEVICE_SERVICE 0xFFFF
#define SSID_CHR       0xFF01
#define PASSWORD_CHR   0xFF02
#define TEAM_CHR       0xFF03
#define DELAY_CHR      0xFF04
#define VOLUME_CHR     0xFF05
#define RESET_CHR      0xFF06

EventGroupHandle_t infoEventGroup;
#define SSID_BIT     BIT2
#define PASSWORD_BIT BIT3
#define TEAM_BIT     BIT4
#define DELAY_BIT    BIT5

bool reset = false;

User_Info *info_buffer;
uint8_t ble_addr_type;

void ble_app_advertise(void);

static int user_ssid_readWrite(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg) 
{
    switch(ctxt->op) {
        case BLE_GATT_ACCESS_OP_READ_CHR:
            os_mbuf_append(ctxt->om, info_buffer->wifi_ssid, sizeof(info_buffer->wifi_ssid));
            return 0;

        case BLE_GATT_ACCESS_OP_WRITE_CHR:
            if (ctxt->om->om_len < 35) {
                memcpy(info_buffer->wifi_ssid, ctxt->om->om_data, ctxt->om->om_len);
                info_buffer->wifi_ssid[ctxt->om->om_len] = '\x0';
                printf("Incoming message: %s\n", info_buffer->wifi_ssid);
                xEventGroupSetBits(infoEventGroup, SSID_BIT);
            } else
                ESP_LOGW(TAG, "Attempted to write wifi SSID longer than allowed leength");
            return 0;

        default:
            ESP_LOGE(TAG, "user_ssid_readWrite Failed!");
    }

    return 0;
}

static int user_password_readWrite(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    switch(ctxt->op) {
        case BLE_GATT_ACCESS_OP_READ_CHR:
            os_mbuf_append(ctxt->om, info_buffer->wifi_password, sizeof(info_buffer->wifi_password));
            return 0;

        case BLE_GATT_ACCESS_OP_WRITE_CHR:
            if (ctxt->om->om_len < 65) {
                memcpy(info_buffer->wifi_password, ctxt->om->om_data, ctxt->om->om_len);
                info_buffer->wifi_password[ctxt->om->om_len] = '\x0';
                printf("Incoming message: %s\n", info_buffer->wifi_password);
                xEventGroupSetBits(infoEventGroup, PASSWORD_BIT);
            } else 
                ESP_LOGW(TAG, "Attempted to write wifi password longer than allowed length");
            return 0;

        default:
            ESP_LOGE(TAG, "user_password_readWrite Failed!");
    }

    return 0;
}

static int user_team_readWrite(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    switch(ctxt->op) {
        case BLE_GATT_ACCESS_OP_READ_CHR:
            os_mbuf_append(ctxt->om, info_buffer->team, sizeof(info_buffer->team));
            return 0;

        case BLE_GATT_ACCESS_OP_WRITE_CHR:
            if (ctxt->om->om_len < 50) {
                memcpy(info_buffer->team, ctxt->om->om_data, ctxt->om->om_len);
                info_buffer->team[ctxt->om->om_len] = '\x0';

                cJSON *team_object = cJSON_Parse(info_buffer->team);
                if (team_object == NULL)
                {
                    const char *err = cJSON_GetErrorPtr();
                    if (err)
                    {
                        ESP_LOGE(TAG, "Error parsing json before %s", err);
                        return 1;
                    }
                }

                cJSON *team_name = cJSON_GetObjectItemCaseSensitive(team_object, "name");
                memcpy(info_buffer->team_name, team_name->valuestring, strlen(team_name->valuestring));
                info_buffer->team_name[strlen(team_name->valuestring)] = '\x0';

                printf("Incoming message: %s\n", info_buffer->team);
                printf("Incoming message: %s\n", info_buffer->team_name);
                xEventGroupSetBits(infoEventGroup, TEAM_BIT);
            } else
                ESP_LOGW(TAG, "Attempted to write team object longer than allowed length");
            return 0;

        default:
            ESP_LOGE(TAG, "user_team_readWrite Failed!");
    }

    return 0;
}

static int user_delay_readWrite(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    switch(ctxt->op) {
        case BLE_GATT_ACCESS_OP_READ_CHR:
            os_mbuf_append(ctxt->om, info_buffer->delay, sizeof(info_buffer->delay));
            return 0;

        case BLE_GATT_ACCESS_OP_WRITE_CHR:
            if (ctxt->om->om_len < 5) {
                memcpy(info_buffer->delay, ctxt->om->om_data, ctxt->om->om_len);
                info_buffer->delay[ctxt->om->om_len] = '\x0';
                printf("Incoming message: %s\n", info_buffer->delay);
                xEventGroupSetBits(infoEventGroup, DELAY_BIT);
            } else 
                ESP_LOGW(TAG, "Attempted to write delay value longer than allowed length");
            return 0;

        default:
            ESP_LOGE(TAG, "user_delay_readWrite Failed!");
    }

    return 0;
}

static int user_volume_readWrite(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    switch(ctxt->op) {
        case BLE_GATT_ACCESS_OP_READ_CHR:
            os_mbuf_append(ctxt->om, info_buffer->volume, sizeof(info_buffer->volume));
            return 0;

        case BLE_GATT_ACCESS_OP_WRITE_CHR:
            if (ctxt->om->om_len < 7) {
                char volume_value[7]; 
                memcpy(volume_value, ctxt->om->om_data, ctxt->om->om_len);
                volume_value[ctxt->om->om_len] = '\x0';
                printf("Incoming message: %s\n", volume_value);

                if (strcmp(volume_value, "Test") == 0) {
                    printf("Testing Volume Setting\n");
                    play_wav_file(info_buffer->volume, 1);
                }
                else {
                    memcpy(info_buffer->volume, ctxt->om->om_data, ctxt->om->om_len);
                    info_buffer->volume[ctxt->om->om_len] = '\x0';
                }
            } else 
                ESP_LOGW(TAG, "Attempted to write volume value longer than allowed length");
            return 0;

        default:
            ESP_LOGE(TAG, "user_volume_readWrite Failed!");
    }

    return 0;
}

static int reset_device(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    char incoming_message[6];
    incoming_message[ctxt->om->om_len] = '\x0';
    memcpy(incoming_message, ctxt->om->om_data, ctxt->om->om_len);
    printf("Incoming message: %s\n", incoming_message);
    if (strcmp(incoming_message, "reset") == 0) {
        reset = true;
        xEventGroupSetBits(infoEventGroup, SSID_BIT | PASSWORD_BIT | TEAM_BIT | DELAY_BIT);
    }
    else
        ESP_LOGE(TAG, "reset_device Failed!");

    return 0;
}

static const struct ble_gatt_svc_def gatt_svcs[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(DEVICE_SERVICE),
        .characteristics = (struct ble_gatt_chr_def[])
        {
            {
                .uuid = BLE_UUID16_DECLARE(SSID_CHR),
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,
                .access_cb = user_ssid_readWrite
            },
            {
                .uuid = BLE_UUID16_DECLARE(PASSWORD_CHR),
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,
                .access_cb = user_password_readWrite
            }, 
            {
                .uuid = BLE_UUID16_DECLARE(TEAM_CHR),
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,
                .access_cb = user_team_readWrite
            },
            {
                .uuid = BLE_UUID16_DECLARE(DELAY_CHR),
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,
                .access_cb = user_delay_readWrite
            },
            {
                .uuid = BLE_UUID16_DECLARE(VOLUME_CHR),
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,
                .access_cb = user_volume_readWrite
            },
            {
                .uuid = BLE_UUID16_DECLARE(RESET_CHR),
                .flags = BLE_GATT_CHR_F_WRITE,
                .access_cb = reset_device
            },
            {0}
        }
    }, {0}
};

static int ble_gap_event(struct ble_gap_event *event, void *arg)
{
    switch (event->type)
    {
        case BLE_GAP_EVENT_CONNECT:
            ESP_LOGI(TAG, "BLE GAP EVENT CONNECT %s", event->connect.status == 0 ? "OK!" : "FAILED!");
            if (event->connect.status != 0)
                ble_app_advertise();
            else {
                const char *ble_connected_text = "Connected\n\nWaiting for user data. . .";
                set_oled_text(ble_connected_text);
            }
            break;
        case BLE_GAP_EVENT_DISCONNECT:
            ESP_LOGI(TAG, "BLE GAP EVENT DISCONNECT");
            ble_app_advertise();
            break;
        case BLE_GAP_EVENT_ADV_COMPLETE:
            ESP_LOGI(TAG, "BLE ADV COMPLETE");
            ble_app_advertise();
            break;
        case BLE_GAP_EVENT_SUBSCRIBE:
            ESP_LOGI(TAG, "BLE GAP EVENT SUBSCRIBE");
            break;
    }

    return 0;
}

void ble_app_advertise(void)
{
    const char *ble_text = "Waiting for BLE connection. . .";
    set_oled_text(ble_text);

    struct ble_hs_adv_fields fields;
    memset(&fields, 0, sizeof(fields));
    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_DISC_LTD;
    fields.tx_pwr_lvl_is_present = 1;
    fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;
    const char *device_name;
    device_name = ble_svc_gap_device_name();
    fields.name= (uint8_t *)device_name;
    fields.name_len = strlen(device_name);
    fields.name_is_complete = 1;
    ble_gap_adv_set_fields(&fields);

    struct ble_gap_adv_params adv_params;
    memset(&adv_params, 0, sizeof(adv_params));
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;

    ble_gap_adv_start(ble_addr_type, NULL, BLE_HS_FOREVER, &adv_params, ble_gap_event, NULL);
}

void ble_app_on_sync(void)
{
    ble_hs_id_infer_auto(0, &ble_addr_type);
    ble_app_advertise();
}

void host_task(void *param)
{
    nimble_port_run();

    nimble_port_freertos_deinit();
}

void run_ble(User_Info *passed_structure)
{
    info_buffer = passed_structure;

    nimble_port_init();

    ble_svc_gap_device_name_set(DEVICE_NAME);
    ble_svc_gap_init();
    ble_svc_gatt_init();

    ble_gatts_count_cfg(gatt_svcs);
    ble_gatts_add_svcs(gatt_svcs);

    ble_hs_cfg.sync_cb = ble_app_on_sync;
    nimble_port_freertos_init(host_task);
}

void stop_ble()
{
    int rc = nimble_port_stop();
    if (rc == 0)
        nimble_port_deinit();
    else
        ESP_LOGI(TAG, "nimble_port_stop failed");
}

esp_err_t all_values_set()
{
    infoEventGroup = xEventGroupCreate();
    EventBits_t info_bits = xEventGroupWaitBits(infoEventGroup, SSID_BIT | PASSWORD_BIT | TEAM_BIT | DELAY_BIT, true, true, portMAX_DELAY);
    if (info_bits) {
        if (reset)
            return ESP_RESET;
        else {
            const char *save_user_info_text = "Saving user info. . .";
            set_oled_text(save_user_info_text);
            return ESP_OK;
        }
    }
    else {
        ESP_LOGE(TAG, "UNEXPECTED ERROR GETTING USER INFO BITS");
        return ESP_FAIL;
    }
}