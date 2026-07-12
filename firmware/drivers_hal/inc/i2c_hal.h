#ifndef I2C_HAL_H
#define I2C_HAL_H

/** @defgroup hal HAL
 *  @brief Hardware Abstraction Layer.
 *  @{
 *  @defgroup template_hal Template HAL
 *  @brief Template driver for ESP32-C6.
 *  @{
 *
 * @section genDesc General Description
 * 
 * This file is the template for developing a new Hardware Abstraction Layer (HAL)
 * driver. It wraps the low-level ESP-IDF peripheral API and exposes it through
 * a hardware-independent interface.
 *
 * @author Matias Tripode
 *
 * @section changelog
 *
 * |   Date     | Description                                            |
 * |:----------:|:-------------------------------------------------------|
 * | DD/MM/AAAA | Document creation                                      |
 *
 **/

/*==================[inclusions]=============================================*/
#include <stdbool.h>
#include <stdint.h>
#include <gpio_hal.h>

/*==================[macros]=================================================*/

/*==================[typedef]================================================*/
/**
 * @brief TODO: Define the data types needed for this HAL driver.
 *
 * Examples:
 *   - An enum of available peripheral instances (channels, ports, interfaces).
 *   - A configuration struct passed to the Init function.
 */
typedef enum {
    I2C_OK         = 0,       /*!< esp_err_t value indicating success (no error) */
    I2C_FAIL       = -1,      /*!< Generic esp_err_t code indicating failure */
    I2C_ERR_INVALID_ARG         = -2,   /*!< Invalid argument */
    I2C_ERR_INVALID_STATE       = -3   /*!< Invalid state */
} i2c_hal_error_t;

typedef enum {
    I2C_HAL_CONTROLLER_0 = 0,
    I2C_HAL_CONTROLLER_COUNT = 1,
} i2c_hal_controller_t;

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/
/**
 * @brief Initialize the peripheral.
 *
 * TODO: Describe what this function configures (clock, GPIO muxing, mode, etc.)
 *
 * @return uint8_t true if initialization was successful, false otherwise.
 */
i2c_hal_error_t I2CHalInit(i2c_hal_controller_t controller, uint8_t device_address, uint32_t freq, gpio_t sda, gpio_t scl);

i2c_hal_error_t I2CHalDeinit(void);
/**
 * @brief TODO: Add the remaining public functions of this driver.
 *
 * Follow the naming convention: ModuleNameAction()
 * Examples: UartHalWrite(), AdcHalRead(), TimerHalStart()
 */

 i2c_hal_error_t I2CHalWrite(i2c_hal_controller_t controller, uint8_t dev_addr,
                    uint8_t reg_addr, const uint8_t *data, size_t len);

                    /**
 * @brief TODO: Add the remaining public functions of this driver.
 *
 * Follow the naming convention: ModuleNameAction()
 * Examples: UartHalWrite(), AdcHalRead(), TimerHalStart()
 */
i2c_hal_error_t I2CHalRead(i2c_hal_controller_t controller, uint8_t dev_addr,
                   uint8_t reg_addr, uint8_t *data, size_t len);

                   /**
 * @brief TODO: Add the remaining public functions of this driver.
 *
 * Follow the naming convention: ModuleNameAction()
 * Examples: UartHalWrite(), AdcHalRead(), TimerHalStart()
 */
void I2CHalDelayUs(uint32_t us);
/**
 * @brief TODO: Add the remaining public functions of this driver.
 *
 * Follow the naming convention: ModuleNameAction()
 * Examples: UartHalWrite(), AdcHalRead(), TimerHalStart()
 */
void I2CHalDelayMs(uint32_t ms);



#endif /* #ifndef I2C_HAL_H */

/*==================[end of file]============================================*/
