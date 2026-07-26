#ifndef BMI270_CONFIG_H
#define BMI270_CONFIG_H

/**
 * @file bmi270_config.h
 * @brief Declaraciones del archivo de configuración oficial del BMI270.
 *
 * El driver carga este bloque en la memoria interna del sensor durante
 * BMI270Init(). Su contenido es generado por Bosch y no debe modificarse.
 */

#include <stddef.h>
#include <stdint.h>

extern const uint8_t bmi270_config_file[]; /**< Bytes de configuración del sensor. */
extern const size_t bmi270_config_file_size; /**< Tamaño del bloque, en bytes. */

#endif /* BMI270_CONFIG_H */
