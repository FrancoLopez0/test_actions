# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2 CACHE STRING "Board type")

cmake_minimum_required(VERSION 3.13)

# Include the Pico SDK import script
# (This file is expected to be created as part of the setup)
include(pico_sdk_import.cmake)

project(pico2_template C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the Pico SDK
pico_sdk_init()

# Placeholder add_subdirectory calls for the project structure
add_subdirectory(src/hal)
add_subdirectory(src/middleware)
add_subdirectory(src/app)
add_subdirectory(libs)
add_subdirectory(third-party)

# Testing subdirectories
# add_subdirectory(test/SIL)
add_subdirectory(test/HIL)
