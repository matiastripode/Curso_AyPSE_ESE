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

  /** @name Configuración I2C del BMI270
   * @{
   */
  #define GPIO_BMI270_SDA          GPIO_20  /**< Línea de datos I2C. */
  #define GPIO_BMI270_SCL          GPIO_21  /**< Línea de reloj I2C. */
  #define BMI270_I2C_FREQUENCY_HZ  400000U  /**< Frecuencia del bus: 400 kHz. */
  #define BMI270_CHIP_ID 0x24  /**< Chip ID correspondiente al sensor BMI270. */
  /** @} */
/*==================[typedef]================================================*/

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/** @} */
/** @} */

#endif /* BOARD_CONFIG_H */

/*==================[end of file]============================================*/
