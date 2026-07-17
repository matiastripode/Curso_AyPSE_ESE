#ifndef HAL_DELAY_H
#define HAL_DELAY_H

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
 * @author Nombre Apellido
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
/*==================[macros]=================================================*/

/*==================[typedef]================================================*/
/**
 * @brief TODO: Define the data types needed for this HAL driver.
 *
 * Examples:
 *   - An enum of available peripheral instances (channels, ports, interfaces).
 *   - A configuration struct passed to the Init function.
 */

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/
/**
 * @brief Initialize the peripheral.
 *
 * TODO: Describe what this function configures (clock, GPIO muxing, mode, etc.)
 *
 * @return uint8_t true if initialization was successful, false otherwise.
 */
// precise busy-wait
// large values block the CPU and should be avoided
void HalDelayUs(uint32_t us);

// cooperative FreeRTOS task delay
// must be called from task context, not an ISR.
void HalDelayMs(uint32_t ms);

/**
 * @brief TODO: Add the remaining public functions of this driver.
 *
 * Follow the naming convention: ModuleNameAction()
 * Examples: UartHalWrite(), AdcHalRead(), TimerHalStart()
 */

/** @} */
/** @} */

#endif /* #ifndef HAL_DELAY_H */

/*==================[end of file]============================================*/
