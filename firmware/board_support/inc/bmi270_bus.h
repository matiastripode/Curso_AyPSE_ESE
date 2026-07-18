#ifndef BMI270_BUS_H
#define BMI270_BUS_H

/**
 * @file bmi270_bus.h
 * @brief Contrato de transporte abstracto utilizado por el driver BMI270.
 *
 * La interfaz permite conectar el driver con I2C, SPI o una implementación
 * fake para tests sin introducir dependencias de una HAL concreta.
 *
 * @author Matias Tripode
 * @ingroup bmi270_bsp
 */

#include <stddef.h>
#include <stdint.h>

/**
 * @defgroup bmi270_bus Bus abstracto BMI270
 * @brief Operaciones de transporte requeridas por el driver BMI270.
 * @ingroup bmi270_bsp
 * @{
 */

/**
 * @brief Resultados devueltos por las implementaciones del bus BMI270.
 */
typedef enum {
    BMI270_BUS_OK = 0,                /**< Operación completada correctamente. */
    BMI270_BUS_FAIL = -1,             /**< Error de comunicación o error no clasificado. */
    BMI270_BUS_ERR_INVALID_ARG = -2,  /**< Uno o más argumentos son inválidos. */
    BMI270_BUS_ERR_INVALID_STATE = -3 /**< El transporte no está en el estado requerido. */
} bmi270_bus_error_t;

/**
 * @brief Interfaz de transporte inyectada al driver BMI270.
 *
 * @note El objeto apuntado por @p context debe permanecer válido durante
 * toda la vida útil de esta interfaz.
 */
typedef struct {
    void *context; /**< Contexto opaco perteneciente a la implementación concreta. */

    /**
     * @brief Lee bytes consecutivos a partir de un registro del sensor.
     *
     * @param[in] context Contexto de la implementación concreta.
     * @param[in] reg Dirección del primer registro que se desea leer.
     * @param[out] data Buffer de destino.
     * @param[in] len Cantidad de bytes solicitados; debe ser mayor que cero.
     * @return Resultado de la operación de bus.
     */
    bmi270_bus_error_t (*read)(void *context, uint8_t reg,
                               uint8_t *data, size_t len);

    /**
     * @brief Escribe bytes consecutivos a partir de un registro del sensor.
     *
     * @param[in] context Contexto de la implementación concreta.
     * @param[in] reg Dirección del primer registro que se desea escribir.
     * @param[in] data Buffer con los datos que se transmitirán.
     * @param[in] len Cantidad de bytes que se escribirán; debe ser mayor que cero.
     * @return Resultado de la operación de bus.
     */
    bmi270_bus_error_t (*write)(void *context, uint8_t reg,
                                const uint8_t *data, size_t len);

    /**
     * @brief Realiza el retardo bloqueante solicitado por el sensor.
     *
     * @param[in] us Duración mínima del retardo, en microsegundos.
     */
    void (*delay_us)(uint32_t us);
} bmi270_bus_t;

/** @} */

#endif /* BMI270_BUS_H */
