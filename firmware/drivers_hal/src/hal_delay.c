/**
 * @file hal_delay.c
 * @brief Implementación de retardos breves para ESP32-C6.
 * @author Matias Tripode
 */

/*==================[inclusions]=============================================*/
#include "hal_delay.h"
#include "esp_rom_sys.h"
/* TODO: Include the specific ESP-IDF driver header for this peripheral.
 * This is the ONLY place in the project where ESP-IDF low-level headers
 * should be included. Examples:
 *   #include "driver/uart.h"
 *   #include "driver/spi_master.h"
 *   #include "esp_adc/adc_oneshot.h"
 */
/*==================[macros and definitions]=================================*/
/* TODO: Define private constants and internal data types here.
 * These are only visible within this file.
 * Example:
 *   #define BUFFER_SIZE 256
 *
 *   typedef struct {
 *       bool initialized;
 *   } template_handle_t;
 */

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

void HalDelayUs(uint32_t us) {
    if (us == 0) {
        return;
    }
    esp_rom_delay_us(us);
}

/*==================[end of file]============================================*/
