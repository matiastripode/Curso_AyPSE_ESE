#ifndef BLE_IMU_H
#define BLE_IMU_H

/** @defgroup middleware Middleware
 *  @brief Layer of intermediate logical services.
 *  @{
 *  @defgroup animation_middleware Animation Middleware
 *  @brief LEDs animation driver.
 *  @{
 * 
 * @section genDesc General Description
 * 
 * This middleware provides a set of animation functions for the LEDs on the board.
 * It allows you to create different animation patterns for the LEDs.
 * 
 * @author 
 *
 * @section changelog
 *
 * |   Date	    | Description                                    						|
 * |:----------:|:----------------------------------------------------------------------|
 * | 23/10/2023 | Document creation		                         						|
 * 
 **/

/*==================[inclusions]=============================================*/
#include <stdbool.h>
#include <stdint.h>
/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

typedef enum {
    BLE_IMU_OK = 0,
    BLE_IMU_NOT_CONNECTED = 1,
    BLE_IMU_NOT_SUBSCRIBED = 2,
    BLE_IMU_ERR_INVALID_ARG = -1,
    BLE_IMU_ERR_INVALID_STATE = -2,
    BLE_IMU_ERR_NO_MEMORY = -3,
    BLE_IMU_ERR_STACK = -4
} ble_imu_error_t;

typedef struct {
    uint32_t timestamp_ms;
    int16_t x;
    int16_t y;
    int16_t z;
} ble_imu_accel_sample_t;

ble_imu_error_t BLEIMUInit(void);
ble_imu_error_t BLEIMUPublishAccel(const ble_imu_accel_sample_t *sample);

#endif /* #ifndef BLE_IMU_H */

/*==================[end of file]============================================*/

