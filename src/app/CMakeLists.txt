add_library(app STATIC
    firmware.c
    app_blinky.c
    app_blinky.h
)

target_include_directories(app PUBLIC
    .
)

# Link against the middleware layer
target_link_libraries(app PUBLIC
    middleware
    freertos
    pico_multicore
)

# Create the final binary for the Pico 2
add_executable(pico2_template
    main.c
)

# Set the output directory to the build root for VS Code extension compatibility
set_target_properties(pico2_template PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

target_link_libraries(pico2_template
    app
    hal
    freertos
    pico_multicore
)

# Enable standard Pico SDK binary output (UF2, etc.)
pico_add_extra_outputs(pico2_template)

# Enable USB output (Serial) and disable UART output
pico_enable_stdio_usb(pico2_template 1)
pico_enable_stdio_uart(pico2_template 0)
