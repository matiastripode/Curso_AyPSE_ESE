#ifndef BLE_IMU_H
#define BLE_IMU_H

/**
 * @file ble_imu.h
 * @brief Servicio BLE para publicar muestras del acelerómetro.
 *
 * El módulo expone una característica GATT de lectura y notificación con la
 * última muestra recibida. El payload tiene 14 bytes y se codifica en
 * little-endian:
 *
 * | Offset | Campo          | Tamaño |
 * |:------:|----------------|:------:|
 * | 0      | Versión (1)    | 1 byte |
 * | 1      | Flags reservados | 1 byte |
 * | 2      | Secuencia      | 2 bytes |
 * | 4      | Timestamp en ms| 4 bytes |
 * | 8      | Eje X en LSB   | 2 bytes |
 * | 10     | Eje Y en LSB   | 2 bytes |
 * | 12     | Eje Z en LSB   | 2 bytes |
 *
 * @author Matias Tripode
 */

/** @defgroup middleware Middleware
 *  @brief Servicios de lógica intermedia.
 *  @{
 *  @defgroup ble_imu_middleware BLE IMU
 *  @brief Publicación de muestras inerciales mediante BLE.
 *  @{
 **/

/*==================[inclusions]=============================================*/
#include <stdbool.h>
#include <stdint.h>
/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

/*==================[external data declaration]==============================*/

/** @brief Resultados de las operaciones del servicio BLE IMU. */
typedef enum {
    BLE_IMU_OK = 0,                  /**< Operación completada correctamente. */
    BLE_IMU_NOT_CONNECTED = 1,       /**< La muestra se guardó, pero no hay conexión. */
    BLE_IMU_NOT_SUBSCRIBED = 2,      /**< La muestra se guardó, pero las notificaciones están deshabilitadas. */
    BLE_IMU_ERR_INVALID_ARG = -1,    /**< Uno o más argumentos son inválidos. */
    BLE_IMU_ERR_INVALID_STATE = -2,  /**< El servicio todavía no fue inicializado. */
    BLE_IMU_ERR_NO_MEMORY = -3,      /**< No fue posible crear la tarea del host. */
    BLE_IMU_ERR_STACK = -4           /**< La pila NimBLE rechazó la operación. */
} ble_imu_error_t;

/** @brief Muestra cruda del acelerómetro que se publicará por BLE. */
typedef struct {
    uint32_t timestamp_ms; /**< Instante de adquisición, en milisegundos. */
    int16_t x;             /**< Aceleración sobre el eje X, en LSB. */
    int16_t y;             /**< Aceleración sobre el eje Y, en LSB. */
    int16_t z;             /**< Aceleración sobre el eje Z, en LSB. */
} ble_imu_accel_sample_t;

/*==================[external functions declaration]=========================*/
/**
 * @brief Inicializa NimBLE, registra el servicio GATT e inicia su tarea.
 *
 * Las llamadas posteriores no reinicializan el servicio.
 *
 * @retval BLE_IMU_OK El servicio quedó inicializado o ya lo estaba.
 * @retval BLE_IMU_ERR_NO_MEMORY No fue posible crear la tarea del host.
 * @retval BLE_IMU_ERR_STACK Falló la inicialización de NimBLE o del servicio GATT.
 */
ble_imu_error_t BLEIMUInit(void);

/**
 * @brief Actualiza la última muestra y, si corresponde, la notifica.
 *
 * La muestra queda disponible para lectura GATT incluso cuando no existe una
 * conexión o el cliente no está suscripto. Cada llamada incrementa el número
 * de secuencia del payload.
 *
 * @param[in] sample Muestra que se serializará; no puede ser NULL.
 *
 * @retval BLE_IMU_OK La notificación fue enviada.
 * @retval BLE_IMU_NOT_CONNECTED No hay un cliente conectado.
 * @retval BLE_IMU_NOT_SUBSCRIBED El cliente no habilitó notificaciones.
 * @retval BLE_IMU_ERR_INVALID_ARG @p sample es NULL.
 * @retval BLE_IMU_ERR_INVALID_STATE El servicio no está inicializado.
 * @retval BLE_IMU_ERR_STACK NimBLE no pudo enviar la notificación.
 */
ble_imu_error_t BLEIMUPublishAccel(const ble_imu_accel_sample_t *sample);

/** @} */
/** @} */

#endif /* #ifndef BLE_IMU_H */

/*==================[end of file]============================================*/
