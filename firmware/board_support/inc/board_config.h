#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

/** @defgroup bsp BSP
 *  @brief Board support package layer.
 *  @{
 *  @defgroup board_config Board Configuration
 *  @brief Pin mapping and board configuration
 *  @{
 */

/*==================[inclusions]=============================================*/
#include "gpio_hal.h"

/*==================[macros]=================================================*/
/** @name Mapeo de Pines de LEDs
 *  @{
 */
#define GPIO_LED1   GPIO_4  /**< LED 1 (Green) */
#define GPIO_LED2   GPIO_5  /**< LED 2 (Yellow) */
#define GPIO_LED3   GPIO_6  /**< LED 3 (Red) */
/** @} */

/*==================[typedef]================================================*/

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/** @} */
/** @} */

#endif /* BOARD_CONFIG_H */

/*==================[end of file]============================================*/
