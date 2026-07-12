#include "unity.h"
#include "i2c_hal.h"

TEST_CASE("I2CHalInit configures I2C without error", "[drivers_hal][i2c]")
{
    uint8_t bmi270_address = 0x68;
    i2c_hal_error_t result = I2CHalInit(I2C_HAL_CONTROLLER_0, bmi270_address, 400000, GPIO_20, GPIO_21);
    TEST_ASSERT_TRUE(result == I2C_OK);
}


TEST_CASE("I2CHalInit is idempotent with same params", "[drivers_hal][i2c]")
{
    uint8_t bmi270_address = 0x68;
    i2c_hal_error_t result = I2CHalInit(I2C_HAL_CONTROLLER_0, bmi270_address, 400000, GPIO_20, GPIO_21);
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

TEST_CASE("I2CHalRead configures I2C without error", "[drivers_hal][i2c]")
{
    uint8_t bmi270_address = 0x68;
    I2CHalInit(I2C_HAL_CONTROLLER_0, bmi270_address, 400000, GPIO_20, GPIO_21);
    uint8_t chip_id;
    i2c_hal_error_t result = I2CHalRead(I2C_HAL_CONTROLLER_0, bmi270_address, 0x00,  &chip_id, 1);
    TEST_ASSERT_TRUE(result == I2C_OK);
    TEST_ASSERT_TRUE(chip_id ==  0x24);
}



