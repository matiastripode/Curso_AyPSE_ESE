#ifndef BMI270_H
#define BMI270_H

/**
 * @file bmi270.h
 * @brief Interfaz pública del driver BSP para el sensor inercial BMI270.
 *
 * Este módulo contiene la lógica propia del BMI270 y accede al hardware
 * exclusivamente mediante la interfaz abstracta @ref bmi270_bus_t.
 *
 * @author Matias Tripode
 * @ingroup bsp
 */

#include <stdint.h>

#include "bmi270_bus.h"

/**
 * @brief Resultados devueltos por las implementaciones del bus BMI270.
 */
typedef enum {
    BMI270_OK = 0,                /**< Operación completada correctamente. */
    BMI270_FAIL = -1,             /**< Error de comunicación o error no clasificado. */
    BMI270_ERR_INVALID_ARG = -2,  /**< Uno o más argumentos son inválidos. */
    BMI270_ERR_INVALID_STATE = -3 /**< El transporte no está en el estado requerido. */
} bmi270_error_t;


typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} bmi270_vector_t;

/**
 * @defgroup bmi270_bsp Driver BMI270
 * @brief Driver BSP independiente del transporte para el sensor BMI270.
 * @ingroup bsp
 * @{
 */

/**
 * @brief Inicializa el sensor BMI270 mediante el bus suministrado.
 *
 * El bus y su contexto deben permanecer válidos mientras el driver pueda
 * utilizarlos. Antes de iniciar cualquier transferencia, la función debe
 * comprobar que @p bus y sus callbacks `read`, `write` y `delay_us` no sean
 * nulos.
 *
 * @param[in] bus Interfaz de transporte previamente creada e inicializada.
 *
 * @return Valor distinto de cero si la inicialización fue exitosa; cero en
 * caso de error.
 *
 * @warning La secuencia de inicialización del sensor todavía se encuentra en
 * desarrollo y esta API no debe considerarse funcional en el estado actual.
 */
bmi270_error_t BMI270Init(const bmi270_bus_t *bus);

/** @} */


bmi270_error_t BMI270AccelRead(bmi270_vector_t *accel);

#endif /* BMI270_H */
