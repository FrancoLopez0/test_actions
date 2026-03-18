add_library(hal STATIC
    hal_common.h
    hal_gpio.c
    hal_gpio.h
)

target_include_directories(hal PUBLIC
    .
)

# Link against Pico SDK core library
target_link_libraries(hal PUBLIC
    pico_stdlib
)

# Conditional linking for Wi-Fi support (Pico 2W models)
if (PICO_CYW43_SUPPORTED)
    target_link_libraries(hal PUBLIC
        pico_cyw43_arch_none
    )
endif()
