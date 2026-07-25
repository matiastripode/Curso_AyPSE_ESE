/**
 * 
 */

/*==================[inclusions]=============================================*/
#include <string.h>
#include "ble_imu.h"
#include "esp_log.h"
#include "nimble/nimble_port.h"
#include "nimble/ble.h"
#include "host/ble_hs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "host/util/util.h"
/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/
static const char *TAG = "BLE_IMU";
static bool initialized = false;
static TaskHandle_t BLEIMUHostTaskHandle = NULL;
#define BLE_IMU_PAYLOAD_SIZE 14U
static uint8_t latest_payload[BLE_IMU_PAYLOAD_SIZE];
static uint16_t accel_value_handle;

static uint16_t connection_handle = BLE_HS_CONN_HANDLE_NONE;
static bool notifications_enabled;
static uint8_t own_address_type;
/*==================[internal functions declaration]=========================*/
// Callback function declarations
static int gatt_svr_access_cb(uint16_t conn_handle, uint16_t attr_handle,
    struct ble_gatt_access_ctxt *ctxt, void *arg);
static int BLEIMUGapEvent(struct ble_gap_event *event, void *arg);

static void BLEIMUHostTask(void *pvParameters) {
    ESP_LOGI(TAG, "NimBLE host task started");
    nimble_port_run(); // //This function will return only when nimble_port_stop() is executed.

    // Defensive programming: tasks must delete themselves if they break out of the loop
    vTaskDelete(NULL); 
}

static int BLEIMUStartAdvertising(void) {
    struct ble_hs_adv_fields fields = {0};
    struct ble_gap_adv_params adv_params = {0};
    fields.flags = BLE_HS_ADV_F_DISC_GEN |
                    BLE_HS_ADV_F_BREDR_UNSUP;
    fields.tx_pwr_lvl_is_present = 1;
    fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;
    fields.name = (const uint8_t *)ble_svc_gap_device_name();
    fields.name_len = strlen((const char *)fields.name);
    fields.name_is_complete = 1;
    // 
    int rc = ble_gap_adv_set_fields(&fields);

    if (rc != 0) {
        ESP_LOGE(TAG, "failed to set advertising fields; rc=%d", rc);
        return rc;
    }
    //
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    /* Start advertising */
    rc = ble_gap_adv_start(own_address_type, NULL, BLE_HS_FOREVER, &adv_params,
                        BLEIMUGapEvent, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to start advertising, error code: %d", rc);
        return rc;
    }
    ESP_LOGI(TAG, "advertising started!");
    return 0;

}
static void BLEIMUOnSync(void) {
    int rc = 0;
    //
    rc = ble_hs_util_ensure_addr(0);
    if (rc != 0) {
        ESP_LOGE(TAG, "device does not have any available bt address! error code: %d", rc);
        return;
    }
    rc = ble_hs_id_infer_auto(0, &own_address_type);
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to infer address type, error code: %d", rc);
        return;
    }
    // Se reiniciará advertising.
    rc = BLEIMUStartAdvertising();
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to start advertising; rc=%d", rc);
        return;
    }
    ESP_LOGI(TAG,  "NimBLE host synchronized");
}

static void BLEIMUOnReset(int reason) {
    ESP_LOGW(TAG, "NimBLE host reset; reason=%d", reason);
}

static void gatt_svr_subscribe_cb(struct ble_gap_event *event) {
    /* Check connection handle */
    if (event->subscribe.conn_handle != BLE_HS_CONN_HANDLE_NONE) {
        ESP_LOGI(TAG, "subscribe event; conn_handle=%d attr_handle=%d",
                event->subscribe.conn_handle, event->subscribe.attr_handle);
    } else {
        ESP_LOGI(TAG, "subscribe by nimble stack; attr_handle=%d",
                event->subscribe.attr_handle);
    }

    /* Check attribute handle */
    if (event->subscribe.attr_handle == accel_value_handle && 
        event->subscribe.conn_handle == connection_handle) {
        notifications_enabled = event->subscribe.cur_notify;
    }
}
static int BLEIMUGapEvent(struct ble_gap_event *event, void *arg) {
    int rc = 0;
    switch (event->type) {
        case BLE_GAP_EVENT_CONNECT:
            ESP_LOGI(TAG, "connect status=%d handle=%d",
               event->connect.status,
               event->connect.conn_handle);
            if (event->connect.status == 0) {
                connection_handle = event->connect.conn_handle;
                notifications_enabled = false;
            } else {
                // Se reiniciará advertising.
                rc = BLEIMUStartAdvertising();
                if (rc != 0) {
                    ESP_LOGE(TAG, "failed to start advertising; rc=%d", rc);
                }
            }
            return 0;
        case BLE_GAP_EVENT_DISCONNECT:
             ESP_LOGI(TAG, "disconnected; reason=%d",
               event->disconnect.reason);
            connection_handle = BLE_HS_CONN_HANDLE_NONE;
            notifications_enabled = false;
            // Se reiniciará advertising.
            rc = BLEIMUStartAdvertising();
            if (rc != 0) {
                ESP_LOGE(TAG, "failed to start advertising; rc=%d", rc);
            }
            return 0;
        case BLE_GAP_EVENT_SUBSCRIBE:
            /* Print subscription info to log */
            ESP_LOGI(TAG,
                "subscribe event; conn_handle=%d attr_handle=%d "
                "reason=%d prevn=%d curn=%d previ=%d curi=%d",
                event->subscribe.conn_handle, event->subscribe.attr_handle,
                event->subscribe.reason, event->subscribe.prev_notify,
                event->subscribe.cur_notify, event->subscribe.prev_indicate,
                event->subscribe.cur_indicate);
            /* GATT subscribe event callback */
            gatt_svr_subscribe_cb(event);
            return 0;
        case BLE_GAP_EVENT_ADV_COMPLETE:
            return 0;
        default:
            return 0;
    }
}
/*==================[internal data definition]===============================*/

static const ble_uuid128_t gatt_svr_svc_uuid =
    BLE_UUID128_INIT(
        0x00, 0x27, 0x9c, 0x1a,
        0x6b, 0x5d, 0xe1, 0xa7,
        0x76, 0x4b, 0x4a, 0xfc,
        0x01, 0x00, 0x2f, 0x6d
    );
static const ble_uuid128_t gatt_svr_chr_uuid =
    BLE_UUID128_INIT(
        0x00, 0x27, 0x9c, 0x1a,
        0x6b, 0x5d, 0xe1, 0xa7,
        0x76, 0x4b, 0x4a, 0xfc,
        0x02, 0x00, 0x2f, 0x6d
    );

static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &gatt_svr_svc_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[]) { {
            .uuid = &gatt_svr_chr_uuid.u,
            .access_cb = gatt_svr_access_cb, // Handlers for read/notification
            .val_handle = &accel_value_handle,
            .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
        }, {
            0, /* No more characteristics in this service */
        } },
    },
    {
        0, /* No more services */
    },
};

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

static int send_custom_notification(uint16_t conn_handle, uint16_t attr_handle, uint8_t *data, uint16_t len) {
    struct os_mbuf *om;
    int rc;

    // Allocate and copy flat data into an mbuf packet buffer
    om = ble_hs_mbuf_from_flat(data, len);
    if (!om) {
        return BLE_HS_ENOMEM; // Memory allocation failed
    }

    // Send the custom notification
    // Note: ble_gatts_notify_custom consumes the mbuf `om` regardless of outcome
    rc = ble_gatts_notify_custom(conn_handle, attr_handle, om);
    
    if (rc != 0) {
        // Handle transmission error (e.g., queue full or disconnected)
        return rc;
    }

    return 0;
}

static int gatt_svr_access_cb(uint16_t conn_handle, uint16_t attr_handle,
                              struct ble_gatt_access_ctxt *ctxt, void *arg) 
{
    int rc = 0;
    switch (ctxt->op) {
        case BLE_GATT_ACCESS_OP_READ_CHR:
            rc = os_mbuf_append(ctxt->om, latest_payload, sizeof(latest_payload));
            if (rc != 0) {
                ESP_LOGE(TAG,  "GATT copying latest data failed");
                return BLE_ATT_ERR_INSUFFICIENT_RES;
            }
            return 0;
        default:
            return BLE_ATT_ERR_UNLIKELY;
    }
}

static ble_imu_error_t BLERegisterServiceGATT() {
    
    int rc;

    // Reset runtime GATT tracking configurations
    rc = ble_gatts_reset();
    if (rc != 0) {
        return rc;
    }
    // Register standard device and identity services (GAP)
    ble_svc_gap_init();

    rc = ble_svc_gap_device_name_set("BMI270-IMU");
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to set GAP device name; rc=%d", rc);
        return BLE_IMU_ERR_STACK;
    }
    ble_svc_gatt_init();

    // Inject your custom array definition into the host stack
    rc = ble_gatts_count_cfg(gatt_svr_svcs);
    if(rc != 0) {
        return BLE_IMU_ERR_STACK;
    }

    rc = ble_gatts_add_svcs(gatt_svr_svcs);
    if(rc != 0) {
        return BLE_IMU_ERR_STACK;
    }
    return BLE_IMU_OK;
}
/*==================[external functions definition]==========================*/

ble_imu_error_t BLEIMUInit(void) {
    if (initialized == true) {
        return BLE_IMU_OK;
    }
    // 1.
    int result_nimble = nimble_port_init();
    if (result_nimble != 0) {
        ESP_LOGE(TAG,  "nimble_port_init Failed");
        return BLE_IMU_ERR_STACK;
    }
    // 2. asigna los dos callbacks en ble_hs_cfg y crea la tarea. configurar ble_hs_cfg.sync_cb y reset_cb
    ble_hs_cfg.sync_cb  = BLEIMUOnSync;
    ble_hs_cfg.reset_cb = BLEIMUOnReset;
    // 3. 
    ble_imu_error_t ble_gatt_result = BLERegisterServiceGATT();
    if (ble_gatt_result != BLE_IMU_OK) {
        ESP_LOGE(TAG,  "GATT initialization failed");
        return BLE_IMU_ERR_STACK;
    }

    // 3. 
    BaseType_t result = xTaskCreate(
        BLEIMUHostTask,      // Function pointer
        "BLEIMUHostTask",    // Text name for debugging
        4096,                // Stack depth (in bytes)
        NULL,                // Parameter passed to task
        2,                   // Task priority
        &BLEIMUHostTaskHandle        // Task handle storage
    );

    // 4. Verify successful creation before starting the scheduler
    if (result != pdPASS) {
        BLEIMUHostTaskHandle = NULL;
        result_nimble = nimble_port_deinit();
        if (result_nimble != 0) {
            ESP_LOGE(TAG,  "nimble_port_deinit Failed");
        }
        return BLE_IMU_ERR_NO_MEMORY;
    }
  
    initialized = true;
    ESP_LOGI(TAG,  "Initialized Successfully");
    return BLE_IMU_OK;
}

ble_imu_error_t BLEIMUPublishAccel(const ble_imu_accel_sample_t *sample) {
    return BLE_IMU_OK;
}

/*==================[end of file]============================================*/
