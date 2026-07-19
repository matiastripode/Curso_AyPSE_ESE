idf_component_register(SRCS "test_gpio_hal.c"
                            "test_i2c_hal.c"
                       REQUIRES unity drivers_hal main
                       WHOLE_ARCHIVE)
