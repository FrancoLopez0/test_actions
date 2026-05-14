#ifndef HAL_COMMON_H
#define HAL_COMMON_H

/**
 * @file hal_common.h
 * @brief Common include for the Hardware Abstraction Layer (HAL).
 * 
 * This file provides standard Pico SDK includes required by the HAL implementation.
 */


#ifndef SIL_TESTING
    #include "pico/stdlib.h"
    #include "hardware/gpio.h"
    #include "hardware/sync.h"
#endif
/* Common HAL definitions/macros can be added here */

#include <stdint.h>
#include <stdbool.h>

#endif /* HAL_COMMON_H */
