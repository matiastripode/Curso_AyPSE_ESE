/**
 * @file bmi270.c
 * @author Matias Tripode
 * @brief Implementación del driver BSP para el sensor BMI270.
 * @version 0.1
 * @date 2026-07-18
 * @ingroup bmi270_bsp
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include "bmi270.h"
#include "bmi270_bus.h"
#include "board_config.h"
/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

/**
 * @brief Inicializa el BMI270 mediante el bus inyectado.
 *
 * @note Implementación pendiente. La secuencia definitiva validará los
 * callbacks, ejecutará el reinicio del sensor, comprobará el identificador y
 * cargará su bloque de configuración.
 */

 /*
  1. Validar bus, context y los tres callbacks.
  2. Guardar o copiar la interfaz para uso posterior.
  3. Hacer el dummy read requerido después del encendido.
  4. Escribir 0xB6 en CMD (0x7E).
  5. Esperar al menos 2 ms usando delay_us.
  6. Leer CHIP_ID (0x00).
  7. Comprobar que sea 0x24.
 */
bmi270_error_t BMI270Init(const bmi270_bus_t *bus) {
	if (bus == NULL || bus->context  == NULL || bus->read == NULL || bus->write == NULL || bus->delay_us == NULL) {
        printf("BMI270: Failed to initialize bus. Error: null param\n");
        return  BMI270_ERR_INVALID_ARG;
    }

    const uint8_t command_register = 0x7E;
    const uint8_t soft_reset_command = 0xB6;
    uint8_t chip_id = 0;

    bmi270_bus_error_t result = bus->write(bus->context, command_register, &soft_reset_command, 1);

    if (result != BMI270_BUS_OK) {
		printf("BMI270: Failed to initialize bmi270. Error: writing soft_reset command\n");
        return  BMI270_ERR_INVALID_STATE;
	}

    bus->delay_us(2000);
    
    result = bus->read(bus->context, 0x00, &chip_id, 1);
	if (result != BMI270_BUS_OK) {
		printf("BMI270: Failed to initialize bmi270. Error: reading chip_id\n");
        return  BMI270_ERR_INVALID_STATE;
	}
	if (chip_id !=  BMI270_CHIP_ID) {
		printf("BMI270: Failed to initialize bmi270. Error: wrong chip_id\n");
        return  BMI270_FAIL;
	}

	return BMI270_OK;
}

/*==================[end of file]============================================*/
