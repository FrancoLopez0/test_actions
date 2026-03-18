# SIL testing using Unity
cmake_minimum_required(VERSION 3.10)
project(SIL_Tests C)

# Create the test executable
add_executable(test_blinky 
    test_blinky.c
    ../../src/middleware/mw_led.c
    ../../third-party/unity/src/unity.c
)

# Include the headers from the source layers and the mocks
# Using CMAKE_CURRENT_SOURCE_DIR to be safe since this might be run as a subproject or standalone
target_include_directories(test_blinky PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/hal
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/middleware
    ${CMAKE_CURRENT_SOURCE_DIR}/mock
    ${CMAKE_CURRENT_SOURCE_DIR}/../../third-party/unity/include
)

# Add to CTest
enable_testing()
add_test(NAME SIL_Blinky COMMAND test_blinky)
