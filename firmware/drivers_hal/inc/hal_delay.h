#ifndef HAL_DELAY_H
#define HAL_DELAY_H

/**
 * @file hal_delay.h
 * @brief Interfaz de abstracción para retardos breves de hardware.
 *
 * @author Matias Tripode
 * @ingroup hal
 */

#include <stdint.h>

/**
 * @defgroup hal_delay Delay HAL
 * @brief Retardos bloqueantes independientes de un periférico particular.
 * @ingroup hal
 * @{
 */

/**
 * @brief Realiza una espera activa expresada en microsegundos.
 *
 * La función bloquea la CPU durante el intervalo solicitado y puede demorar
 * algo más que @p us. Está destinada a esperas breves exigidas por el hardware,
 * por ejemplo durante la inicialización o el reinicio de un sensor. Para
 * esperas prolongadas debe utilizarse un mecanismo de temporización del RTOS.
 *
 * @param[in] us Duración mínima solicitada, en microsegundos. El valor cero no
 * produce espera.
 *
 * @note Esta función realiza busy-wait y no cede la CPU a otras tareas.
 */
void HalDelayUs(uint32_t us);

/** @} */

#endif /* HAL_DELAY_H */
