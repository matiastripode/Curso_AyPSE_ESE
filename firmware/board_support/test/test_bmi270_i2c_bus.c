#include "unity.h"
#include "bmi270_i2c_bus.h"
#include "test_helpers.h"


/*
 Test case:
  1. Inicializar la HAL con 0x68, 400 kHz, SDA GPIO20 y SCL GPIO21.
  2. Crear el proxy.
  3. Verificar que todos los callbacks quedaron configurados.
  4. Declarar uint8_t chip_id.
  5. Ejecutar bus.read(bus.context, 0x00, &chip_id, 1).
  6. Verificar que la lectura retorna true.
  7. Verificar chip_id == 0x24.
  8. Liberar la HAL al final para no afectar los tests posteriores.
*/
TEST_CASE("BMI270I2CBus read without error", "[board_support][bmi270_i2c_bus]")
{
    bmi270_bus_t          bus;
    const uint8_t bmi270_address = 0x68;
    bmi270_i2c_bus_context_t context = {
        .controller = I2C_HAL_CONTROLLER_0,
        .dev_addr = bmi270_address
    };    
    uint8_t chip_id = 0;

    bmi270_bus_error_t result = BMI270I2CBusCreate(&bus, &context);
    if (result == BMI270_BUS_OK) {
        TestMarkI2CBusAcquired();
    }

    TEST_ASSERT_NOT_NULL(bus.read);
    TEST_ASSERT_NOT_NULL(bus.write);
    TEST_ASSERT_NOT_NULL(bus.delay_us);
    TEST_ASSERT_TRUE(result == BMI270_BUS_OK);

    result = bus.read(bus.context, 0x00, &chip_id, 1);

    TEST_ASSERT_TRUE(result == BMI270_BUS_OK);
    TEST_ASSERT_TRUE(chip_id ==  0x24);
}


/*
  initialize BMI270
  write 0xB6 to CMD register 0x7E
  wait at least 2 ms
  read CHIP_ID register 0x00
  expect 0x24
*/
TEST_CASE("BMI270I2CBus write without error", "[board_support][bmi270_i2c_bus]")
{
    bmi270_bus_t          bus;
    const uint8_t bmi270_address = 0x68;
    const uint8_t command_register = 0x7E;
    const uint8_t soft_reset_command = 0xB6;
    uint8_t chip_id = 0;
    bmi270_i2c_bus_context_t context = {
        .controller = I2C_HAL_CONTROLLER_0,
        .dev_addr = bmi270_address
    };    

    bmi270_bus_error_t result = BMI270I2CBusCreate(&bus, &context);
    if (result == BMI270_BUS_OK) {
        TestMarkI2CBusAcquired();
    }

    TEST_ASSERT_NOT_NULL(bus.read);
    TEST_ASSERT_NOT_NULL(bus.write);
    TEST_ASSERT_NOT_NULL(bus.delay_us);
    TEST_ASSERT_TRUE(result == BMI270_BUS_OK);

    result = bus.write(bus.context, command_register, &soft_reset_command, 1);

    TEST_ASSERT_TRUE(result == BMI270_BUS_OK);

    bus.delay_us(2000);
    
    result = bus.read(bus.context, 0x00, &chip_id, 1);

    TEST_ASSERT_TRUE(result == BMI270_BUS_OK);
    TEST_ASSERT_TRUE(chip_id ==  0x24);
}
