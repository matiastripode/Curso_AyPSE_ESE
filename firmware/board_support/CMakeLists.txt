idf_build_get_property(target IDF_TARGET)

# Always compiled source files
set(srcs "src/led.c" "src/bmi270_i2c_bus.c" "src/bmi270.c" "src/bmi270_config.c"
    )

# Always included headers
set(includes "inc")

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${includes}
                       REQUIRES drivers_hal)