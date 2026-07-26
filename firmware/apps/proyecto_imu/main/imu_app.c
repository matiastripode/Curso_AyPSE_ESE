/*! @mainpage Monitor BLE del BMI270
 *
 * @section genDesc Descripción general
 *
 * Inicializa NVS, el servicio BLE y el acelerómetro BMI270. Cuando existe una
 * muestra nueva, informa sus ejes y magnitud y publica los datos mediante una
 * característica GATT. El muestreo se realiza cada 10 ms.
 *
 * Si NimBLE está deshabilitado en la configuración de ESP-IDF, la aplicación
 * solamente registra una advertencia.
 *
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include "sdkconfig.h"
#include "nvs_flash.h"
#if CONFIG_BT_NIMBLE_ENABLED
    #include "ble_imu.h"
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include "bmi270.h"
    #include "bmi270_bus.h"
    #include "bmi270_i2c_bus.h"
    #include "esp_timer.h"
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include <math.h>
#endif
#include "esp_log.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/
static const char *TAG = "IMU_APP";
/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]==========================*/
/**
 * @brief Punto de entrada de la aplicación de adquisición y publicación IMU.
 */
void app_main(void){
	esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

#if CONFIG_BT_NIMBLE_ENABLED 
    bmi270_bus_t bmi270_bus;
    bmi270_i2c_bus_context_t bmi270_context = {
        .controller = I2C_HAL_CONTROLLER_0,
        .dev_addr = 0x68
    };
    ble_imu_error_t ble_result = BLEIMUInit();

    if (ble_result != BLE_IMU_OK) {
        ESP_LOGE(TAG, "BLE initialization failed: %d", ble_result);
        return;
    }

    bmi270_bus_error_t bus_result = BMI270I2CBusCreate(&bmi270_bus, &bmi270_context);

    if (bus_result != BMI270_BUS_OK) {
        ESP_LOGE(TAG, "BMI270 I2C bus initialization failed: %d",
                bus_result);
        return;
    }
    bmi270_error_t bmi_result = BMI270Init(&bmi270_bus);

    if (bmi_result != BMI270_OK) {
        ESP_LOGE(TAG, "BMI270 initialization failed: %d", bmi_result);
        return;
    }

    bmi270_vector_t accel = {0};

    while (true) {
        bmi_result = BMI270AccelTryRead(&accel);

        if (bmi_result == BMI270_OK) {
            ble_imu_accel_sample_t sample = {
                .timestamp_ms = (uint32_t)(esp_timer_get_time() / 1000ULL),
                .x = accel.x,
                .y = accel.y,
                .z = accel.z
            };
            float magnitude = sqrtf(
                (float)accel.x * accel.x +
                (float)accel.y * accel.y +
                (float)accel.z * accel.z
            );

            ESP_LOGI(TAG, "x=%d y=%d z=%d magnitude=%.1f LSB (%.3f g)",
                    accel.x,
                    accel.y,
                    accel.z,
                    magnitude,
                    magnitude / 16384.0f);
           
            ble_imu_error_t publish_result =
                BLEIMUPublishAccel(&sample);

            if (publish_result < 0) {
                ESP_LOGE(TAG, "BLE publish failed: %d", publish_result);
            }
        } else if (bmi_result != BMI270_NOT_READY) {
            ESP_LOGE(TAG, "BMI270 read failed: %d", bmi_result);
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
#else
    ESP_LOGW(TAG, "NimBLE is disabled; BLE IMU was not initialized");
#endif
}
/*==================[end of file]============================================*/
