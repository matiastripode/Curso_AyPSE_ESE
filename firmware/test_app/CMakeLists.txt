# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

list(APPEND EXTRA_COMPONENT_DIRS "../middleware")
list(APPEND EXTRA_COMPONENT_DIRS "../board_support")
list(APPEND EXTRA_COMPONENT_DIRS "../drivers_hal")

# Restrict compiled components to isolate the framework and test targets.
set(COMPONENTS main middleware board_support drivers_hal)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(test_app)
