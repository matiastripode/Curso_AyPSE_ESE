#include "unity.h"
#include "bmi270.h"
#include "bmi270_bus.h"
#include "bmi270_i2c_bus.h"
#include "test_helpers.h"

TEST_CASE("BMI270 init", "[board_support][bmi270]")
{
    bmi270_bus_t          bus;
    const uint8_t bmi270_address = 0x68;
    bmi270_i2c_bus_context_t context = {
        .controller = I2C_HAL_CONTROLLER_0,
        .dev_addr = bmi270_address
    };    

    bmi270_bus_error_t i2c_result = BMI270I2CBusCreate(&bus, &context);
    if (i2c_result == BMI270_BUS_OK) {
        TestMarkI2CBusAcquired();
    }

    TEST_ASSERT_NOT_NULL(bus.read);
    TEST_ASSERT_NOT_NULL(bus.write);
    TEST_ASSERT_NOT_NULL(bus.delay_us);
    TEST_ASSERT_TRUE(i2c_result == BMI270_BUS_OK);

    bmi270_error_t result = BMI270Init(&bus);

    TEST_ASSERT_TRUE(result == BMI270_OK);
}
