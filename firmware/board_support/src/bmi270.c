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
#include "bmi270_config.h"

/*==================[macros and definitions]=================================*/
#define PWR_CONF        0x7C
#define INIT_CTRL       0x59
#define INIT_ADDR_0     0x5B
#define INIT_ADDR_1     0x5C
#define INIT_DATA       0x5E
#define DATA_8          0x0C
#define INTERNAL_STATUS 0x21

#define ACC_CONF        0x40
#define ACC_RANGE       0x41
#define PWR_CTRL        0x7D

/* Seis bytes del acelerometro */
#define X_LSB       0x0C
#define X_MSB       0x0D
#define Y_LSB       0x0E
#define Y_MSB       0x0F
#define Z_LSB       0x10
#define Z_MSB       0x11

#define INIT_LOAD_BLOCK 32U
#define ADV_PWR_SAVE_DELAY_US  450U
#define PROC_BLOB_TIME_US      150000U

// Accelerometer configuration
#define BMI270_ACC_ODR_100_HZ       0x08U /* 100 Hz */
#define BMI270_ACC_BWP_NORMAL       0x20U /* Normal mode */
#define BMI270_ACC_FILTER_PERF      0x80U /* Performance mode */

#define BMI270_ACC_RANGE_2G         0x00U /* Rango de medición: ±2 g. */
#define BMI270_PWR_CTRL_ACC_EN      (1U << 2) /* Habilita el acelerómetro. */
#define BMI270_ACC_STARTUP_DELAY_US 2000U /* Tiempo de estabilización. */
/*==================[internal data declaration]==============================*/
static const bmi270_bus_t *bmi270_bus_instance;
static bool initialized;
/*==================[internal functions declaration]=========================*/


/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/
/*
  1. Escribir 0x00 en PWR_CONF para deshabilitar advanced power save.
  2. Esperar al menos 450 us.
  3. Escribir 0x00 en INIT_CTRL.
  4. Recorrer el blob en bloques pares.
  5. Para cada bloque:
      - Calcular la dirección interna.
      - Escribir INIT_ADDR_0 y INIT_ADDR_1.
      - Escribir el bloque en INIT_DATA.

  6. Escribir 0x01 en INIT_CTRL.
  7. Esperar aproximadamente 150 ms.
  8. Leer INTERNAL_STATUS.
  9. Verificar que el estado sea init_ok.
*/
static bmi270_error_t BMI270LoadConfig(const bmi270_bus_t *bus) {
    const uint8_t config_load_disable = 0x00U;
    const uint8_t config_load_enable = 0x01U;
    uint8_t internal_status = 0U;

    /* La memoria de inicialización no puede cargarse mientras advanced power
     * save está activo. El valor cero también deshabilita FIFO self-wakeup. */
    bmi270_bus_error_t result = bus->write(
        bus->context,
        PWR_CONF,
        &config_load_disable,
        sizeof(config_load_disable));
    if (result != BMI270_BUS_OK) {
        return BMI270_FAIL;
    }

    /* Tiempo mínimo para que el sensor abandone advanced power save. */
    bus->delay_us(ADV_PWR_SAVE_DELAY_US);

    /* INIT_CTRL=0 prepara la memoria interna para recibir el archivo. */
	result = bus->write(
        bus->context,
        INIT_CTRL,
        &config_load_disable,
        sizeof(config_load_disable));
    if (result != BMI270_BUS_OK) {
        return BMI270_FAIL;
    }

    for (size_t index = 0;
         index < bmi270_config_file_size;
         index += INIT_LOAD_BLOCK) {
        uint8_t init_address[2];

        /* INIT_ADDR utiliza direcciones de palabras de 16 bits, mientras
         * index representa una posición expresada en bytes. */
        size_t word_address = index / 2U;
        init_address[0] = (uint8_t)(word_address & 0x0FU);
        init_address[1] = (uint8_t)(word_address >> 4);

        /* Los dos bytes escritos desde INIT_ADDR_0 también alcanzan el
         * registro consecutivo INIT_ADDR_1. */
        result = bus->write(
            bus->context,
            INIT_ADDR_0,
            init_address,
            sizeof(init_address));
        if (result != BMI270_BUS_OK) {
            return BMI270_FAIL;
        }

        /* Este cálculo también admite un último bloque menor que 32 bytes. */
        size_t remaining = bmi270_config_file_size - index;
        size_t write_size = remaining < INIT_LOAD_BLOCK ? remaining : INIT_LOAD_BLOCK;

        /* Se transmite directamente desde flash, sin una copia temporal. */
        result = bus->write(
            bus->context,
            INIT_DATA,
            &bmi270_config_file[index],
            write_size);
        if (result != BMI270_BUS_OK) {
            return BMI270_FAIL;
        }
    }

    /* INIT_CTRL=1 ordena procesar el archivo que acaba de transferirse. */
    result = bus->write(
        bus->context,
        INIT_CTRL,
        &config_load_enable,
        sizeof(config_load_enable));
    if (result != BMI270_BUS_OK) {
        return BMI270_FAIL;
    }

    /* El procesamiento interno puede necesitar aproximadamente 150 ms. */
    bus->delay_us(PROC_BLOB_TIME_US);

    result = bus->read(
        bus->context,
        INTERNAL_STATUS,
        &internal_status,
        sizeof(internal_status));
    if (result != BMI270_BUS_OK) {
        return BMI270_FAIL;
    }

    /* El valor 0x01 del campo message confirma initialization OK. */
    if ((internal_status & 0x0FU) != 0x01U) {
        printf("BMI270: Configuration load failed. INTERNAL_STATUS=0x%02X\n",
               internal_status);
        return BMI270_ERR_INVALID_STATE;
    }

    return BMI270_OK;
}

/*
  1. Configurar ACC_CONF.
  2. Seleccionar el rango en ACC_RANGE.
  3. Habilitar acc_en mediante PWR_CTRL.
  4. Esperar la estabilización.
  5. Opcionalmente releer los registros para confirmar la configuración.
*/

static bmi270_error_t BMI270AccelConfig(const bmi270_bus_t *bus) {
    uint8_t acc_conf = BMI270_ACC_ODR_100_HZ | BMI270_ACC_BWP_NORMAL | BMI270_ACC_FILTER_PERF;
    uint8_t acc_range = BMI270_ACC_RANGE_2G;
    uint8_t pwr_ctrl = 0U;

    /* Configura ODR=100 Hz, filtro normal y modo de rendimiento. */
    bmi270_bus_error_t result = bus->write(
        bus->context,
        ACC_CONF,
        &acc_conf,
        sizeof(acc_conf));
    if (result != BMI270_BUS_OK) {
        return BMI270_FAIL;
    }

    /* Selecciona ±2 g. En este rango la escala raw es 16384 LSB/g. */
    result = bus->write(
        bus->context,
        ACC_RANGE,
        &acc_range,
        sizeof(acc_range));
    if (result != BMI270_BUS_OK) {
        return BMI270_FAIL;
    }

    /* Read-modify-write conserva el estado del giróscopo, sensor auxiliar y
     * temperatura si estas funciones se habilitan en el futuro. */
    result = bus->read(
        bus->context,
        PWR_CTRL,
        &pwr_ctrl,
        sizeof(pwr_ctrl));
    if (result != BMI270_BUS_OK) {
        return BMI270_FAIL;
    }

    pwr_ctrl |= BMI270_PWR_CTRL_ACC_EN;

    result = bus->write(
        bus->context,
        PWR_CTRL,
        &pwr_ctrl,
        sizeof(pwr_ctrl));
    if (result != BMI270_BUS_OK) {
        return BMI270_FAIL;
    }

    /* Evita leer muestras mientras el acelerómetro todavía está arrancando. */
    bus->delay_us(BMI270_ACC_STARTUP_DELAY_US);

    /* Comprabar valor de ACC_CONF */
    result = bus->read(
        bus->context,
        ACC_CONF,
        &acc_conf,
        sizeof(acc_conf));
    if (result != BMI270_BUS_OK) {
        return BMI270_FAIL;
    }

    /* Comprabar valor de ACC_CONF */
    result = bus->read(
        bus->context,
        ACC_RANGE,
        &acc_range,
        sizeof(acc_range));
    if (result != BMI270_BUS_OK) {
        return BMI270_FAIL;
    }

    /* Comprabar valor de PWR_CTRL */
    result = bus->read(
        bus->context,
        PWR_CTRL,
        &pwr_ctrl,
        sizeof(pwr_ctrl));
    if (result != BMI270_BUS_OK) {
        return BMI270_FAIL;
    }

    if (acc_conf != 0xA8 || (acc_range & 0x03) != 0x00 || (pwr_ctrl & BMI270_PWR_CTRL_ACC_EN) == 0) {
        return BMI270_ERR_INVALID_STATE;
    }

    return BMI270_OK;
}
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
	/* No se expone una instancia parcialmente inicializada si ocurre un error. */
	initialized = false;
	bmi270_bus_instance = NULL;

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
		initialized = false;
        return  BMI270_ERR_INVALID_STATE;
	}

    bus->delay_us(2000);
    
    result = bus->read(bus->context, 0x00, &chip_id, 1);
	if (result != BMI270_BUS_OK) {
		printf("BMI270: Failed to initialize bmi270. Error: reading chip_id\n");
		initialized = false;
        return  BMI270_ERR_INVALID_STATE;
	}
	if (chip_id !=  BMI270_CHIP_ID) {
		printf("BMI270: Failed to initialize bmi270. Error: wrong chip_id\n");
		initialized = false;
        return  BMI270_FAIL;
	}
    bmi270_error_t config_result = BMI270LoadConfig(bus);

    if (config_result != BMI270_OK) {
        printf("BMI270: Failed to initialize bmi270. Error: trying to load config\n");
        return config_result;
    }
    /* Configura el acelerometro */
    bmi270_error_t accel_result = BMI270AccelConfig(bus);
    if (accel_result != BMI270_OK) {
        printf("BMI270: Failed to initialize accelerometer configuration\n");
        return accel_result;
    }

	/* La instancia pasa a estar disponible solo después de validar el blob. */
	bmi270_bus_instance = bus;
	initialized = true;
	return BMI270_OK;
}

bmi270_error_t BMI270AccelRead(bmi270_vector_t *accel) {
    uint8_t accel_data_raw[6];
    if (accel == NULL) {
        return BMI270_ERR_INVALID_ARG;
    }
	if (initialized == false || bmi270_bus_instance == NULL) {
		return  BMI270_ERR_INVALID_STATE;
	}

    /* Leer los seis bytes del acelerometro */
    bmi270_bus_error_t result = bmi270_bus_instance->read(
        bmi270_bus_instance->context,
        DATA_8,
        accel_data_raw,
        sizeof(accel_data_raw));
    if (result != BMI270_BUS_OK) {
        return BMI270_FAIL;
    }

    int16_t raw_x = (int16_t)(
        (uint16_t)accel_data_raw[0] | (uint16_t)accel_data_raw[1] << 8
    );

    int16_t raw_y = (int16_t)(
        (uint16_t)accel_data_raw[2] | (uint16_t)accel_data_raw[3] << 8
    );
    int16_t raw_z = (int16_t)(
        (uint16_t)accel_data_raw[4] | (uint16_t)accel_data_raw[5] << 8
    );
    bmi270_vector_t temp_accel = {
        .x = raw_x,
        .y = raw_y,
        .z = raw_z
    };
    *accel = temp_accel;

	return  BMI270_OK;
}
/*==================[end of file]============================================*/
