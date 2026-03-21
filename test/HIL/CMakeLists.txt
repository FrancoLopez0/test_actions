# HIL testing using Unity (Target-based)

# This CMake file is for building tests that run on the RP2350 hardware.
# It depends on the Pico SDK and the Unity framework.

# Main hardware test
add_executable(test_hw test_hw_gpio.c )
target_link_libraries(test_hw hal pico_stdlib unity)
target_include_directories(test_hw PRIVATE ${PROJECT_SOURCE_DIR}/src/hal ${PROJECT_SOURCE_DIR}/third-party/unity/include)
pico_add_extra_outputs(test_hw)
pico_enable_stdio_usb(test_hw 1)
pico_enable_stdio_uart(test_hw 0)

# Motor hardware test
add_executable(test_motor test_motor.c )
target_link_libraries(test_motor hal middleware pico_stdlib unity)
target_include_directories(test_motor PRIVATE 
    ${PROJECT_SOURCE_DIR}/src/hal 
    ${PROJECT_SOURCE_DIR}/src/middleware
    ${PROJECT_SOURCE_DIR}/third-party/unity/include
)
pico_add_extra_outputs(test_motor)
pico_enable_stdio_usb(test_motor 1)
pico_enable_stdio_uart(test_motor 0)
