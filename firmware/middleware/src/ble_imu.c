/**
 * 
 */

/*==================[inclusions]=============================================*/
#include "ble_imu.h"
#include "esp_log.h"
#include "nimble/nimble_port.h"
#include "host/ble_hs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/
static const char *TAG = "BLE_IMU";
static bool initialized = false;
static TaskHandle_t BLEIMUHostTaskHandle = NULL;
/*==================[internal functions declaration]=========================*/
static void BLEIMUHostTask(void *pvParameters) {
    ESP_LOGI(TAG, "NimBLE host task started");
    nimble_port_run(); // //This function will return only when nimble_port_stop() is executed.

    // Defensive programming: tasks must delete themselves if they break out of the loop
    vTaskDelete(NULL); 
}


static void BLEIMUOnSync(void) {
    ESP_LOGI(TAG,  "NimBLE host synchronized");
}
static void BLEIMUOnReset(int reason) {
    ESP_LOGW(TAG, "NimBLE host reset; reason=%d", reason);
}
/*==================[internal data definition]===============================*/


/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

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
    ble_hs_cfg.sync_cb  = BLEIMUOnSync;
    ble_hs_cfg.reset_cb = BLEIMUOnReset;
    // 2. asigna los dos callbacks en ble_hs_cfg y crea la tarea. configurar ble_hs_cfg.sync_cb y reset_cb
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
