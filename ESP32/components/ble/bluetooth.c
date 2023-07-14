#include <stdio.h>
#include "esp_log.h"
#include "esp_nimble_hci.h"
#include "freertos/event_groups.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#define TAG "BLE"
#define DEVICE_NAME "ESP32"
#define DEVICE_SERVICE 0xFFFF
#define DEVICE_WRITE 0xFF01

EventGroupHandle_t eventGroup;
#define SSID_BIT BIT2

uint8_t ble_addr_type;

void ble_app_advertise(void);

static int user_info_write(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    printf("Incoming message: %.*s\n", ctxt->om->om_len, ctxt->om->om_data);
    xEventGroupSetBits(eventGroup, SSID_BIT);
    return 0;
}

static const struct ble_gatt_svc_def gatt_svcs[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(DEVICE_SERVICE),
        .characteristics = (struct ble_gatt_chr_def[])
        {
            {
                .uuid = BLE_UUID16_DECLARE(DEVICE_WRITE), // Longest write is about 222 bytes
                .flags = BLE_GATT_CHR_F_WRITE,
                .access_cb = user_info_write
            }, {0}
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

void run_ble()
{
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
    eventGroup = xEventGroupCreate();
    EventBits_t bits = xEventGroupWaitBits(eventGroup, SSID_BIT, true, true, portMAX_DELAY);
    if (bits)
        return ESP_OK;
    else {
        ESP_LOGE(TAG, "UNEXPECTED ERROR GETTING USER INFO BITS");
        return ESP_FAIL;
    }
}