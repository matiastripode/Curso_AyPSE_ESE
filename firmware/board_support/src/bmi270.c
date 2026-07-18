/**
 * @file bmi270.c
 * @author Matias Tripode
 * @brief Implementación del driver BSP para el sensor BMI270.
 * @version 0.1
 * @date 2026-07-18
 * @ingroup bmi270_bsp
 */

/*==================[inclusions]=============================================*/
#include "bmi270.h"
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
uint8_t BMI270Init(const bmi270_bus_t *bus) {
	return -1;
}

/*==================[end of file]============================================*/
