/**
 * @file hal_delay.c
 * @brief Implementación de retardos breves para ESP32-C6.
 * @author Matias Tripode
 */

/*==================[inclusions]=============================================*/
#include "hal_delay.h"
#include "esp_rom_sys.h"
/*==================[macros and definitions]=================================*/

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
