cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)
project(test_project C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

pico_sdk_init()

include_directories(${CMAKE_SOURCE_DIR}/inc)

add_library(libimu STATIC src/lsm9ds1.c)
target_include_directories(libimu PUBLIC ${CMAKE_SOURCE_DIR}/inc)
target_link_libraries(libimu pico_stdlib hardware_i2c)

add_executable(imu
	main.c
)
target_link_libraries(imu libimu pico_stdlib hardware_adc)

add_executable(bus_scan
	bus_scan.c
)
target_link_libraries(bus_scan pico_stdlib hardware_i2c)

pico_enable_stdio_usb(imu 1)
pico_enable_stdio_uart(imu 1)

pico_add_extra_outputs(imu)
