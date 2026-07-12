#include "unity.h"
#include "i2c_hal.h"

TEST_CASE("I2CHalInit configures I2C without error", "[drivers_hal][i2c]")
{
    i2c_hal_error_t result = I2CHalInit(I2C_HAL_CONTROLLER_0, 400000, GPIO_20, GPIO_21);
    TEST_ASSERT_TRUE(result == I2C_OK);
}


TEST_CASE("I2CHalInit is idempotent with same params", "[drivers_hal][i2c]")
{
    i2c_hal_error_t result = I2CHalInit(I2C_HAL_CONTROLLER_0, 400000, GPIO_20, GPIO_21);
    TEST_ASSERT_TRUE(result == I2C_OK);
    result = I2CHalInit(I2C_HAL_CONTROLLER_0, 400000, GPIO_20, GPIO_21);
    TEST_ASSERT_TRUE(result == I2C_OK);
}

TEST_CASE("I2CHalInit should fail with CLK == SDA", "[drivers_hal][i2c]")
{
    i2c_hal_error_t result = I2CHalInit(I2C_HAL_CONTROLLER_0, 400000, GPIO_21, GPIO_21);
    TEST_ASSERT_TRUE(result == I2C_ERR_INVALID_ARG);
}


