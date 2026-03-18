#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#include <stdbool.h>
#include <stdint.h>
#include "hal_common.h"

/**
 * @file hal_gpio.h
 * @brief GPIO abstraction for the RP2350 HAL.
 */

typedef enum {
    HAL_GPIO_DIR_IN = 0,
    HAL_GPIO_DIR_OUT = 1
} hal_gpio_dir_t;

/**
 * @brief Initialize a GPIO pin with a specific direction.
 * @param pin The GPIO pin number.
 * @param dir The direction (Input or Output).
 */
void hal_gpio_init(uint32_t pin, hal_gpio_dir_t dir);

/**
 * @brief Write a value to a GPIO pin.
 * @param pin The GPIO pin number.
 * @param value The value to write (true for high, false for low).
 */
void hal_gpio_write(uint32_t pin, bool value);

/**
 * @brief Read the current value of a GPIO pin.
 * @param pin The GPIO pin number.
 * @return The current value of the pin.
 */
bool hal_gpio_read(uint32_t pin);

/**
 * @brief Toggle the state of a GPIO pin.
 * @param pin The GPIO pin number.
 */
void hal_gpio_toggle(uint32_t pin);

#endif /* HAL_GPIO_H */
