idf_build_get_property(target IDF_TARGET)

# Always compiled source files
set(srcs "src/animation.c")

# Always included headers
set(includes "inc")

# Keep bt available so its Kconfig symbols are loaded. Only the BLE IMU source
# is conditional; the Bluetooth component configures itself when disabled.
set(private_requires board_support drivers_hal bt)
if(CONFIG_BT_NIMBLE_ENABLED)
    list(APPEND srcs "src/ble_imu.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${includes}
                       PRIV_REQUIRES ${private_requires})
