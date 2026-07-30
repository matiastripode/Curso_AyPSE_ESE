idf_component_register(SRCS "test_bmi270_fake_bus.c" "bmi270_fake_bus.c" "test_led.c"
                            "test_bmi270_i2c_bus.c"
                            "test_bmi270.c"
                       REQUIRES unity board_support main
                       WHOLE_ARCHIVE)
