/**
 * @file bmi270_i2c_bus.c
 * @brief Implementación del adaptador entre el BMI270 y la HAL de I2C.
 * @author Matias Tripode
 * @version 0.1
 * @date 2026-07-18
 * @ingroup bmi270_i2c_bus
 */

#include "bmi270_i2c_bus.h"
#include "hal_delay.h"
#include <stdio.h>

/** Tiempo máximo de una escritura I2C, expresado en milisegundos. */
#define BMI270_BUS_TIMEOUT_MS 100U

/**
 * @brief Convierte un resultado de la HAL I2C al dominio de errores del bus.
 *
 * @param[in] err Resultado entregado por la HAL I2C.
 * @return Resultado equivalente de tipo @ref bmi270_bus_error_t.
 */
static bmi270_bus_error_t I2CHalMapToBmi270Error(i2c_hal_error_t err) {
    switch (err) {
        case I2C_OK:
            return BMI270_BUS_OK;
        case I2C_ERR_INVALID_ARG:
            return BMI270_BUS_ERR_INVALID_ARG;
        case I2C_ERR_INVALID_STATE:
            return BMI270_BUS_ERR_INVALID_STATE;
        default:
            return BMI270_BUS_FAIL;
    }
}

/**
 * @brief Implementa la lectura de registros para el proxy I2C.
 *
 * @param[in] context Contexto concreto de tipo @ref bmi270_i2c_bus_context_t.
 * @param[in] reg Dirección del primer registro que se desea leer.
 * @param[out] data Buffer de destino.
 * @param[in] len Cantidad de bytes que se desea leer.
 * @return Resultado de la transferencia traducido al dominio del bus BMI270.
 */
static bmi270_bus_error_t hw_read(void *context, uint8_t reg, uint8_t *data, size_t len) {
    if (context == NULL || len == 0 || data == NULL) {
        printf("BMI270-I2C-BUS: Failed to read bus. Error: null param\n");
        return  BMI270_BUS_ERR_INVALID_ARG;
    }
    const bmi270_i2c_bus_context_t *bmi270_i2c_context = context;
    i2c_hal_error_t result =  I2CHalRead(
        bmi270_i2c_context->controller, 
        bmi270_i2c_context->dev_addr, 
        reg,  
        data, 
        len);    
    return I2CHalMapToBmi270Error(result);
}

/**
 * @brief Implementa la escritura de registros para el proxy I2C.
 *
 * @param[in] context Contexto concreto de tipo @ref bmi270_i2c_bus_context_t.
 * @param[in] reg Dirección del primer registro que se desea escribir.
 * @param[in] data Buffer con los bytes que se transmitirán.
 * @param[in] len Cantidad de bytes que se desea escribir.
 * @return Resultado de la transferencia traducido al dominio del bus BMI270.
 */
static bmi270_bus_error_t hw_write(void *context, uint8_t reg, const uint8_t *data, size_t len) {
    if (context == NULL || len == 0 || data == NULL) {
        printf("BMI270-I2C-BUS: Failed to write bus. Error: null param\n");
        return  BMI270_BUS_ERR_INVALID_ARG;
    }
    const bmi270_i2c_bus_context_t *bmi270_i2c_context = context;
    i2c_hal_error_t result =  I2CHalWrite(bmi270_i2c_context->controller, 
        bmi270_i2c_context->dev_addr, 
        reg, 
        data, 
        len, 
        BMI270_BUS_TIMEOUT_MS);    
    return I2CHalMapToBmi270Error(result);
}

/**
 * @brief Delega una espera breve en la HAL de retardos.
 *
 * @param[in] us Duración mínima solicitada, en microsegundos.
 */
static void hw_delay_us(uint32_t us) {
    HalDelayUs(us);
}

/**
 * @brief Inicializa la HAL I2C y conecta sus operaciones con el proxy BMI270.
 *
 * @see BMI270I2CBusCreate()
 */
bmi270_bus_error_t BMI270I2CBusCreate(bmi270_bus_t *bus,
    bmi270_i2c_bus_context_t *context) {
    if (bus == NULL ||context  == NULL ) {
        printf("BMI270-I2C-BUS: Failed to initialize bus. Error: null param\n");
        return  BMI270_BUS_ERR_INVALID_ARG;
    }

    i2c_hal_error_t result = I2CHalInit(context->controller, context->dev_addr, 400000, GPIO_20, GPIO_21);
    if (result != I2C_OK) {
        printf("BMI270-I2C-BUS: Failed to initialize I2C hal.\n");
        return I2CHalMapToBmi270Error(result);
    }
    bus->context = context;
    bus->write = hw_write;
    bus->read = hw_read;
    bus->delay_us = hw_delay_us;
    return I2CHalMapToBmi270Error(result);
}
