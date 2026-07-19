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
    BMI270_NOT_READY = 1,         /**< BMI270 no esta listo todavia. */
    BMI270_FAIL = -1,             /**< Error de comunicación o error no clasificado. */
    BMI270_ERR_INVALID_ARG = -2,  /**< Uno o más argumentos son inválidos. */
    BMI270_ERR_INVALID_STATE = -3 /**< El transporte no está en el estado requerido. */
} bmi270_error_t;


/**
 * @brief Muestra tridimensional sin procesar del acelerómetro.
 *
 * Cada componente contiene el valor con signo de 16 bits entregado por el
 * BMI270. Con la configuración actual de +-2 g, la sensibilidad nominal es de
 * 16384 LSB/g.
 */
typedef struct {
    int16_t x; /**< Aceleración sin procesar sobre el eje X, en LSB. */
    int16_t y; /**< Aceleración sin procesar sobre el eje Y, en LSB. */
    int16_t z; /**< Aceleración sin procesar sobre el eje Z, en LSB. */
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

/**
 * @brief Intenta obtener una nueva muestra del acelerómetro sin esperar.
 *
 * La función consulta el indicador `drdy_acc` del sensor. Si existe una
 * muestra nueva, lee los tres ejes y actualiza @p accel. Si todavía no hay una
 * muestra disponible, retorna @ref BMI270_NOT_READY inmediatamente y no
 * modifica el vector de salida.
 *
 * @pre El sensor debe haberse inicializado correctamente mediante
 *      BMI270Init().
 *
 * @param[out] accel Vector donde se almacenará la muestra sin procesar.
 *
 * @retval BMI270_OK Se obtuvo una muestra y @p accel fue actualizado.
 * @retval BMI270_NOT_READY Todavía no hay una muestra nueva; @p accel no fue
 *         modificado.
 * @retval BMI270_ERR_INVALID_ARG @p accel es NULL.
 * @retval BMI270_ERR_INVALID_STATE El sensor no está inicializado.
 * @retval BMI270_FAIL Ocurrió un error de comunicación con el sensor.
 *
 * @note Esta función no espera a que el sensor produzca una muestra. Sin
 * embargo, las consultas de estado y datos utilizan las transferencias
 * síncronas proporcionadas por el bus.
 */
bmi270_error_t BMI270AccelTryRead(bmi270_vector_t *accel);

/** @} */

#endif /* BMI270_H */
