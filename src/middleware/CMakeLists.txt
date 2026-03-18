add_library(middleware STATIC
    mw_led.c
    mw_led.h
)

target_include_directories(middleware PUBLIC
    .
)

# Link against the HAL layer
target_link_libraries(middleware PUBLIC
    hal
)
