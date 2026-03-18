#ifndef MOCK_PICO_MULTICORE_H
#define MOCK_PICO_MULTICORE_H

#include <stdint.h>
#include <stdbool.h>

void multicore_launch_core1(void (*func)(void));

#endif // MOCK_PICO_MULTICORE_H
