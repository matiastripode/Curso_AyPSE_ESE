#ifndef I2C_HAL_H
#define I2C_HAL_H

/**
 * @file i2c_hal.h
 * @brief Interfaz de abstracción para el controlador maestro I2C.
 *
 * Este módulo abstrae la configuración del bus y las transferencias por
 * registros de un dispositivo I2C. La implementación actual admite un único
 * controlador y un único dispositivo asociado a ese controlador.
 *
 * @author Matias Tripode
 * @ingroup hal
 */

#include <stddef.h>
#include <stdint.h>

#include "gpio_hal.h"

/**
 * @defgroup i2c_hal I2C HAL
 * @brief Abstracción de acceso al bus I2C en modo maestro.
 * @ingroup hal
 * @{
 */

/**
 * @brief Resultados devueltos por la HAL de I2C.
 *
 * Estos valores son propios de la HAL y no deben interpretarse como valores
 * de tipo `esp_err_t`.
 */
typedef enum {
    I2C_OK = 0,                /**< Operación completada correctamente. */
    I2C_FAIL = -1,             /**< Error de comunicación o error interno no clasificado. */
    I2C_ERR_INVALID_ARG = -2,  /**< Uno o más argumentos son inválidos. */
    I2C_ERR_INVALID_STATE = -3 /**< El controlador no está en el estado requerido. */
} i2c_hal_error_t;

/**
 * @brief Controladores I2C disponibles en la HAL.
 */
typedef enum {
    I2C_HAL_CONTROLLER_0 = 0, /**< Controlador I2C principal. */
    I2C_HAL_CONTROLLER_COUNT  /**< Cantidad de controladores disponibles; no es un controlador válido. */
} i2c_hal_controller_t;

/**
 * @brief Inicializa el controlador y registra un dispositivo I2C.
 *
 * Configura el bus en modo maestro y crea el dispositivo identificado por su
 * dirección de 7 bits. Una segunda llamada con la misma configuración es
 * idempotente. Si el controlador ya fue inicializado con otros parámetros, la
 * función rechaza la nueva configuración.
 *
 * @param[in] controller Controlador I2C que se desea inicializar.
 * @param[in] device_address Dirección I2C de 7 bits del dispositivo.
 * @param[in] freq Frecuencia del reloj del dispositivo, expresada en hertz.
 * @param[in] sda GPIO utilizado para la señal de datos SDA.
 * @param[in] scl GPIO utilizado para la señal de reloj SCL.
 *
 * @retval I2C_OK El controlador quedó inicializado o ya tenía la misma configuración.
 * @retval I2C_ERR_INVALID_ARG El controlador o alguno de los parámetros es inválido.
 * @retval I2C_ERR_INVALID_STATE El controlador ya posee otra configuración.
 * @retval I2C_FAIL El driver subyacente no pudo crear el bus o el dispositivo.
 */
i2c_hal_error_t I2CHalInit(i2c_hal_controller_t controller,
                           uint8_t device_address,
                           uint32_t freq,
                           gpio_t sda,
                           gpio_t scl);

/**
 * @brief Libera el dispositivo y el bus I2C inicializados.
 *
 * @retval I2C_OK El dispositivo y el bus fueron liberados correctamente.
 * @retval I2C_ERR_INVALID_STATE La HAL no se encontraba inicializada.
 * @retval I2C_FAIL El driver subyacente no pudo liberar alguno de los recursos.
 */
i2c_hal_error_t I2CHalDeinit(void);

/**
 * @brief Escribe uno o más bytes a partir de un registro del dispositivo.
 *
 * La dirección del registro y los datos se transmiten dentro de una misma
 * transacción I2C, sin copiar el contenido del buffer de entrada.
 *
 * @param[in] controller Controlador I2C previamente inicializado.
 * @param[in] dev_addr Dirección I2C de 7 bits registrada durante la inicialización.
 * @param[in] reg_addr Dirección del primer registro que se desea escribir.
 * @param[in] data Puntero al buffer con los bytes que se transmitirán.
 * @param[in] len Cantidad de bytes contenidos en @p data; debe ser mayor que cero.
 * @param[in] timeout_ms Tiempo máximo de espera de la transferencia, en milisegundos.
 *
 * @retval I2C_OK Escritura completada correctamente.
 * @retval I2C_ERR_INVALID_ARG Dirección, buffer o longitud inválidos.
 * @retval I2C_ERR_INVALID_STATE El controlador no está inicializado o no coincide.
 * @retval I2C_FAIL El dispositivo no respondió, expiró el timeout u ocurrió otro error de bus.
 */
i2c_hal_error_t I2CHalWrite(i2c_hal_controller_t controller,
                            uint8_t dev_addr,
                            uint8_t reg_addr,
                            const uint8_t *data,
                            size_t len,
                            uint32_t timeout_ms);

/**
 * @brief Lee uno o más bytes a partir de un registro del dispositivo.
 *
 * La operación transmite primero la dirección del registro y luego realiza
 * la lectura mediante una transacción combinada con repeated-start. La
 * implementación actual utiliza un timeout interno de 100 ms.
 *
 * @param[in] controller Controlador I2C previamente inicializado.
 * @param[in] dev_addr Dirección I2C de 7 bits registrada durante la inicialización.
 * @param[in] reg_addr Dirección del primer registro que se desea leer.
 * @param[out] data Buffer donde se almacenarán los bytes recibidos.
 * @param[in] len Cantidad de bytes que se desea recibir; debe ser mayor que cero.
 *
 * @retval I2C_OK Lectura completada correctamente.
 * @retval I2C_ERR_INVALID_ARG Dirección, buffer o longitud inválidos.
 * @retval I2C_ERR_INVALID_STATE El controlador no está inicializado o no coincide.
 * @retval I2C_FAIL El dispositivo no respondió, expiró el timeout u ocurrió otro error de bus.
 */
i2c_hal_error_t I2CHalRead(i2c_hal_controller_t controller,
                           uint8_t dev_addr,
                           uint8_t reg_addr,
                           uint8_t *data,
                           size_t len);

/** @} */

#endif /* I2C_HAL_H */
