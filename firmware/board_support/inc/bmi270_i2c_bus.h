#ifndef BMI270_I2C_BUS_H
#define BMI270_I2C_BUS_H

/**
 * @file bmi270_i2c_bus.h
 * @brief Adaptador del bus abstracto BMI270 hacia la HAL de I2C.
 *
 * @author Matias Tripode
 * @ingroup bmi270_bsp
 */

#include <stdint.h>

#include "bmi270_bus.h"
#include "i2c_hal.h"

/**
 * @defgroup bmi270_i2c_bus Adaptador I2C del BMI270
 * @brief Implementación de @ref bmi270_bus_t basada en la HAL de I2C.
 * @ingroup bmi270_bsp
 * @{
 */

/**
 * @brief Estado requerido por la implementación I2C del proxy.
 *
 * La instancia debe permanecer válida mientras el objeto @ref bmi270_bus_t
 * creado a partir de ella pueda utilizarse.
 */
typedef struct {
    i2c_hal_controller_t controller; /**< Controlador I2C utilizado por el sensor. */
    uint8_t dev_addr;                /**< Dirección I2C de 7 bits del BMI270. */
} bmi270_i2c_bus_context_t;

/**
 * @brief Inicializa la HAL I2C y construye el proxy para el BMI270.
 *
 * Al completarse correctamente, configura en @p bus el contexto y los
 * callbacks de lectura, escritura y retardo. La implementación actual utiliza
 * 400 kHz, GPIO20 para SDA y GPIO21 para SCL.
 *
 * @param[out] bus Interfaz que recibirá los callbacks del adaptador.
 * @param[in] context Configuración persistente del controlador y dispositivo.
 *
 * @retval BMI270_BUS_OK Proxy e I2C inicializados correctamente.
 * @retval BMI270_BUS_ERR_INVALID_ARG @p bus o @p context son nulos, o la HAL
 * rechazó algún parámetro.
 * @retval BMI270_BUS_ERR_INVALID_STATE La HAL ya posee una configuración diferente.
 * @retval BMI270_BUS_FAIL Ocurrió otro error al inicializar la HAL.
 */
bmi270_bus_error_t BMI270I2CBusCreate(
    bmi270_bus_t *bus,
    bmi270_i2c_bus_context_t *context);

/** @} */

#endif /* BMI270_I2C_BUS_H */
