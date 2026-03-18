# HIL testing using Unity (Target-based)

# This CMake file is for building tests that run on the RP2350 hardware.
# It depends on the Pico SDK and the Unity framework.

add_executable(test_hw_gpio test_hw_gpio.c)

# Link against the HAL, Pico SDK standard libraries, and Unity
target_link_libraries(test_hw_gpio
    hal
    pico_stdlib
    unity
)

# Include directories for the tests
target_include_directories(test_hw_gpio PRIVATE
    ${PROJECT_SOURCE_DIR}/src/hal
    ${PROJECT_SOURCE_DIR}/third-party/unity/include
)

# Standard Pico SDK setup for an executable
pico_add_extra_outputs(test_hw_gpio)

# Enable USB output (Serial) and disable UART output
pico_enable_stdio_usb(test_hw_gpio 1)
pico_enable_stdio_uart(test_hw_gpio 0)
