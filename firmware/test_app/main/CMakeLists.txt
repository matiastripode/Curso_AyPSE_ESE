idf_component_register(SRCS "test_app_main.c"
                            "../../drivers_hal/test/test_gpio_hal.c"
                            "../../drivers_hal/test/test_i2c_hal.c"
                            "../../board_support/test/test_led.c"
                            "../../board_support/test/test_bmi270_i2c_bus.c"
                            "../../board_support/test/test_bmi270.c"
                        INCLUDE_DIRS "."
                        REQUIRES unity drivers_hal board_support
                        WHOLE_ARCHIVE)
