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

TEST_CASE("BMI270 AccelTryRead", "[board_support][bmi270]")
{
    bmi270_bus_t          bus;
    const uint8_t bmi270_address = 0x68;
    size_t valid_samples = 0U;
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

    bus.delay_us(2000U);

    bmi270_vector_t accel_data = {0};

    for (size_t i = 0; i < 10U; i++) {
      bus.delay_us(10000U);
      result = BMI270AccelTryRead(&accel_data);

      if (result == BMI270_OK) {
        printf(
            "Sample %u: x=%d, y=%d, z=%d\n",
            (unsigned)i,
            (int)accel_data.x,
            (int)accel_data.y,
            (int)accel_data.z
        );
        valid_samples ++;
      }
    }
    
    TEST_ASSERT_TRUE(
      accel_data.x != 0 ||
      accel_data.y != 0 ||
      accel_data.z != 0
    );

    TEST_ASSERT_GREATER_THAN_UINT32(0U, valid_samples);
}