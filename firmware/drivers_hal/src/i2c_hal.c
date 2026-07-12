/**
 * @file i2c_hal.c
 * @author Matias Tripode
 * @brief HAL driver template for ESP32-C6.
 * @version 0.1
 * @date AAAA-MM-DD
 *
 * @copyright Copyright (c) AAAA
 *
 */

/*==================[inclusions]=============================================*/
#include "i2c_hal.h"
#include "driver/i2c_master.h"
#include "esp_log.h"
#include <stdint.h>
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
 typedef struct {
       bool initialized;
	   i2c_master_bus_handle_t i2c_bus_handle;
	   i2c_master_bus_config_t i2c_master_config;
	   i2c_device_config_t dev_cfg;
	   i2c_master_dev_handle_t dev_handle;
} i2c_handle_t;

static i2c_handle_t i2c_handler;

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/
static i2c_hal_error_t I2CHalMapError(esp_err_t err) {
    switch (err) {
        case ESP_OK:
            return I2C_OK;
        case ESP_ERR_INVALID_ARG:
            return I2C_ERR_INVALID_ARG;
        case ESP_ERR_INVALID_STATE:
            return I2C_ERR_INVALID_STATE;
        default:
            return I2C_FAIL;
    }
}

/*==================[external functions definition]==========================*/

/*
  not initialized
      → create bus, save configuration, return I2C_OK

  already initialized with identical configuration
      → do nothing, return I2C_OK

  already initialized with different configuration
      → return I2C_ERR_INVALID_STATE
*/
i2c_hal_error_t I2CHalInit(i2c_hal_controller_t controller, uint8_t device_address, uint32_t freq, gpio_t sda, gpio_t scl) {
	if (controller >= I2C_HAL_CONTROLLER_COUNT) {
		ESP_LOGE("I2C", "Failed to initialize master bus. Error: wrong controller number");
		return I2C_ERR_INVALID_ARG;
	}
	gpio_num_t scl_gpio = (gpio_num_t)scl;
  	gpio_num_t sda_gpio = (gpio_num_t)sda;
	if (scl_gpio == sda_gpio) {
		ESP_LOGE("I2C", "Failed to initialize master bus. SLC == SDA");
		return I2C_ERR_INVALID_ARG;
	}

	// already initialized with identical configuration
	if (i2c_handler.initialized == true && 
		i2c_handler.dev_cfg.scl_speed_hz == freq &&
		i2c_handler.i2c_master_config.i2c_port == controller && 
		i2c_handler.i2c_master_config.scl_io_num == scl_gpio && 
		i2c_handler.i2c_master_config.sda_io_num == sda_gpio) {
		ESP_LOGI("I2C", "Master bus already initialized successfully (no action).");
		return I2C_OK;
	} else if (i2c_handler.initialized == true) { // already initialized with different configuration
		return I2C_ERR_INVALID_STATE;
	}

	i2c_master_bus_config_t config = {
		.clk_source = I2C_CLK_SRC_DEFAULT, // Use default internal clock source
		.i2c_port = controller, // Choose I2C port (0 or 1)
		.scl_io_num = scl_gpio,  // Specify SCL pin
		.sda_io_num = sda_gpio,  // Specify SDA pin
		.glitch_ignore_cnt = 7, // Filter out line glitches
		.flags.enable_internal_pullup = true, // Enable ESP32 internal pull-up resistors
	};
	// 2. Allocate the master bus handle if needed
	i2c_master_bus_handle_t bus_handle;
    esp_err_t ret = i2c_new_master_bus(&config, &bus_handle);

	// 3. Verify driver allocation success
    if (ret == ESP_OK) {
        ESP_LOGI("I2C", "Master bus config & handle initialized successfully.");
		i2c_handler.i2c_master_config = config;
		i2c_handler.i2c_bus_handle = bus_handle;
    } else {
        ESP_LOGE("I2C", "Failed to initialize master bus. Error: %s", esp_err_to_name(ret));
		return I2CHalMapError(ret);
    }

	i2c_device_config_t dev_cfg = {
		.dev_addr_length = I2C_ADDR_BIT_LEN_7, // standard 7-bit addressing
		.device_address = device_address, // Your slave device hardware address
		.scl_speed_hz = freq, // 100 kHz Standard Mode speed
	};

	i2c_master_dev_handle_t dev_handle;
	ret = i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle);
	if (ret == ESP_OK) {
        ESP_LOGI("I2C", "Master bus and I2C device initialized successfully.");
		i2c_handler.dev_cfg = dev_cfg;
		i2c_handler.dev_handle = dev_handle;
		i2c_handler.initialized = true;
    } else {
        ESP_LOGE("I2C", "Failed to initialize I2C device. Error: %s", esp_err_to_name(ret));
    }
	return I2CHalMapError(ret);
}

i2c_hal_error_t I2CHalDeinit(void) {
	if (i2c_handler.initialized == false) {
			ESP_LOGE("I2C", "Won't deinit a non-initialized bus");
			return I2C_ERR_INVALID_STATE;
	}
	esp_err_t ret = i2c_master_bus_rm_device(i2c_handler.dev_handle);
	if (ret != ESP_OK) {
			ESP_LOGE("I2C", "Failed to remove I2C device. Error: %s", esp_err_to_name(ret));
			return I2CHalMapError(ret);
	}
	ret = i2c_del_master_bus(i2c_handler.i2c_bus_handle);
	if (ret == ESP_OK) {
			i2c_handler = (i2c_handle_t){0};
	} else {
			ESP_LOGE("I2C", "Failed to delete I2C master bus. Error: %s", esp_err_to_name(ret));
	}
	return I2CHalMapError(ret);
}

i2c_hal_error_t I2CHalWrite(i2c_hal_controller_t controller, uint8_t dev_addr,
                    uint8_t reg_addr, const uint8_t *data, size_t len) {
	if (i2c_handler.initialized == false || i2c_handler.i2c_master_config.i2c_port != controller) {
		ESP_LOGE("I2C", "Error: Trying to write into a not initilized controller.");
		return I2C_ERR_INVALID_STATE;
	}
	return -1;
}

i2c_hal_error_t I2CHalRead(i2c_hal_controller_t controller, uint8_t dev_addr,
                   uint8_t reg_addr, uint8_t *data, size_t len) {
	if (i2c_handler.initialized == false || i2c_handler.i2c_master_config.i2c_port != controller) {
		ESP_LOGE("I2C", "Error: Trying to read from a not initilized controller.");
		return I2C_ERR_INVALID_STATE;
	}
	if (i2c_handler.dev_cfg.device_address != dev_addr|| dev_addr > 0x7F || len <= 0) {
		ESP_LOGE("I2C", "Error: Invalid arguments.");
		return I2C_ERR_INVALID_ARG;
	}
	uint32_t timeout_ms = 100;
	esp_err_t ret =  i2c_master_transmit_receive(
      i2c_handler.dev_handle,
      &reg_addr,  // pointer to register-address byte
      1,          // transmit exactly one byte
      data,       // destination buffer
      len,
      timeout_ms
  );

	if (ret != ESP_OK) {
 		ESP_LOGE("I2C", "Failed to read from I2C device. Error: %s", esp_err_to_name(ret));
	}
	return I2CHalMapError(ret);
}

void I2CHalDelayUs(uint32_t us) {

}

void I2CHalDelayMs(uint32_t ms) {

}

/*==================[end of file]============================================*/
