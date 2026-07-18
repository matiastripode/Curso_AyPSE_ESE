#include "unity.h"
#include "i2c_hal.h"
#include "hal_delay.h"
#include "test_helpers.h"

TEST_CASE("I2CHalInit configures I2C without error", "[drivers_hal][i2c]")
{
    uint8_t bmi270_address = 0x68;
    i2c_hal_error_t result = I2CHalInit(I2C_HAL_CONTROLLER_0, bmi270_address, 400000, GPIO_20, GPIO_21);
    if (result == I2C_OK) {
        TestMarkI2CBusAcquired();
    }
    TEST_ASSERT_TRUE(result == I2C_OK);
}


TEST_CASE("I2CHalInit is idempotent with same params", "[drivers_hal][i2c]")
{
    uint8_t bmi270_address = 0x68;
    i2c_hal_error_t result = I2CHalInit(I2C_HAL_CONTROLLER_0, bmi270_address, 400000, GPIO_20, GPIO_21);
    if (result == I2C_OK) {
        TestMarkI2CBusAcquired();
    }
    TEST_ASSERT_TRUE(result == I2C_OK);
    result = I2CHalInit(I2C_HAL_CONTROLLER_0, bmi270_address, 400000, GPIO_20, GPIO_21);
    TEST_ASSERT_TRUE(result == I2C_OK);
}

TEST_CASE("I2CHalInit should fail with CLK == SDA", "[drivers_hal][i2c]")
{
    uint8_t bmi270_address = 0x68;
    i2c_hal_error_t result = I2CHalInit(I2C_HAL_CONTROLLER_0, bmi270_address, 400000, GPIO_21, GPIO_21);
    TEST_ASSERT_TRUE(result == I2C_ERR_INVALID_ARG);
}

TEST_CASE("I2CHalRead read from I2C without error", "[drivers_hal][i2c]")
{
    uint8_t bmi270_address = 0x68;
    i2c_hal_error_t result = I2CHalInit(I2C_HAL_CONTROLLER_0, bmi270_address, 400000, GPIO_20, GPIO_21);
    if (result == I2C_OK) {
        TestMarkI2CBusAcquired();
    }
    TEST_ASSERT_EQUAL(I2C_OK, result);
    uint8_t chip_id;
    result = I2CHalRead(I2C_HAL_CONTROLLER_0, bmi270_address, 0x00,  &chip_id, 1);
    TEST_ASSERT_TRUE(result == I2C_OK);
    TEST_ASSERT_TRUE(chip_id ==  0x24);
}

/*
  initialize BMI270
  write 0xB6 to CMD register 0x7E
  wait at least 2 ms
  read CHIP_ID register 0x00
  expect 0x24
*/
TEST_CASE("I2CHalWrite resets BMI270 and device responds", "[drivers_hal][i2c]")
{
    const uint8_t bmi270_address = 0x68;
    const uint8_t command_register = 0x7E;
    const uint8_t soft_reset_command = 0xB6;
    uint8_t chip_id = 0;

    i2c_hal_error_t result = I2CHalInit(I2C_HAL_CONTROLLER_0,
                    bmi270_address,
                    400000,
                    GPIO_20,
                    GPIO_21);
    if (result == I2C_OK) {
        TestMarkI2CBusAcquired();
    }
    TEST_ASSERT_EQUAL(I2C_OK, result);
    
    result = I2CHalWrite(I2C_HAL_CONTROLLER_0,
                        bmi270_address,
                        command_register,
                        &soft_reset_command,
                        1,
                        100);
    TEST_ASSERT_EQUAL(I2C_OK, result);

    HalDelayUs(3000);

    result = I2CHalRead(I2C_HAL_CONTROLLER_0,
                        bmi270_address,
                        0x00,
                        &chip_id,
                        1);
    TEST_ASSERT_EQUAL(I2C_OK, result);
    TEST_ASSERT_EQUAL_HEX8(0x24, chip_id);
}

