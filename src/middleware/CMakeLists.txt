add_library(middleware STATIC
    mw_led.c
    mw_led.h
    motor.c
    motor.h
    logging.c
    logging.h
    common_rtos.c
    common_rtos.h
)

target_include_directories(middleware PUBLIC
    .
    ${CMAKE_SOURCE_DIR}/src
)

# Link against the HAL layer and necessary Pico SDK hardware libraries
target_link_libraries(middleware PUBLIC
    hal
    hardware_pwm
    freertos
)
