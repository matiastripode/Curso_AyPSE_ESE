#ifndef BMI270_FAKE_BUS_H
#define BMI270_FAKE_BUS_H

/**
 * @file bmi270_fake_bus.h
 * @brief Doble de prueba de @ref bmi270_bus_t que emula los registros del BMI270 en memoria.
 *
 * Permite ejercitar `bmi270.c` sin hardware, inyectando fallas de
 * comunicación en un registro puntual y registrando la secuencia de
 * llamadas realizada por el driver.
 *
 * @author Matias Tripode
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "bmi270_bus.h"

#define BMI270_FAKE_BUS_REG_COUNT   256U
#define BMI270_FAKE_BUS_DELAY_LOG   16U

/**
 * @brief Estado y configuración del bus fake del BMI270.
 *
 * El llamador puede escribir directamente en @ref regs antes de crear el bus
 * para pre-sembrar valores (p.ej. CHIP_ID), y consultar los contadores luego
 * de ejercitar el driver.
 */
typedef struct {
    uint8_t regs[BMI270_FAKE_BUS_REG_COUNT]; /**< Registros simulados del sensor. */

    bool has_fail_write_reg;  /**< Indica si @ref fail_write_reg está activo. */
    uint8_t fail_write_reg;   /**< Registro en el que se fuerza una falla de escritura. */
    bool has_fail_read_reg;   /**< Indica si @ref fail_read_reg está activo. */
    uint8_t fail_read_reg;    /**< Registro en el que se fuerza una falla de lectura. */

    bool has_read_override_reg;   /**< Indica si @ref read_override_reg está activo. */
    uint8_t read_override_reg;    /**< Registro cuyo primer byte leído se reemplaza. */
    uint8_t read_override_value;  /**< Valor devuelto en lugar del contenido real del registro. */

    size_t write_count; /**< Cantidad total de escrituras recibidas. */
    size_t read_count;   /**< Cantidad total de lecturas recibidas. */

    uint8_t last_write_reg;    /**< Registro de la última escritura recibida (cualquiera). */
    size_t last_write_len;      /**< Longitud de la última escritura recibida (cualquiera). */

    /**
     * Longitud de la última escritura recibida por cada registro, indexada
     * por dirección. Permite consultar, por ejemplo, el tamaño del último
     * bloque escrito en INIT_DATA aun cuando el driver escriba otros
     * registros después.
     */
    size_t last_write_len_by_reg[BMI270_FAKE_BUS_REG_COUNT];

    uint32_t delay_log[BMI270_FAKE_BUS_DELAY_LOG]; /**< Argumentos recibidos por delay_us(). */
    size_t delay_count; /**< Cantidad de llamadas a delay_us() registradas hasta ahora. */
} bmi270_fake_bus_ctx_t;

/**
 * @brief Reinicia el contexto y siembra los valores por defecto (CHIP_ID, etc.).
 *
 * @param[out] ctx Contexto a inicializar.
 */
void BMI270FakeBusReset(bmi270_fake_bus_ctx_t *ctx);

/**
 * @brief Conecta @p bus con las operaciones del bus fake respaldadas por @p ctx.
 *
 * @param[out] bus Interfaz que recibirá los callbacks del fake.
 * @param[in] ctx Contexto previamente inicializado con @ref BMI270FakeBusReset.
 * @retval BMI270_BUS_OK Siempre que @p bus y @p ctx sean no nulos.
 * @retval BMI270_BUS_ERR_INVALID_ARG @p bus o @p ctx son nulos.
 */
bmi270_bus_error_t BMI270FakeBusCreate(bmi270_bus_t *bus, bmi270_fake_bus_ctx_t *ctx);

#endif /* BMI270_FAKE_BUS_H */
