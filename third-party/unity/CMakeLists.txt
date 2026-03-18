cmake_minimum_required(VERSION 3.10)

# 1. Definir la librería Unity (Archivo fuente central)
add_library(unity STATIC
    src/unity.c
)

# 2. Definir dónde están los headers de Unity
# PUBLIC permite que al vincular 'unity', se encuentre automáticamente "unity.h"
target_include_directories(unity PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 3. Vincular con dependencias básicas
# Unity necesita stdio (pico_stdlib) para imprimir los resultados por serial
target_link_libraries(unity PRIVATE 
    pico_stdlib
)