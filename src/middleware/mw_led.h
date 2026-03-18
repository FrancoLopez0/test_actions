#ifndef MW_LED_H
#define MW_LED_H

#include <stdint.h>
#include "hal_gpio.h"

/**
 * @file mw_led.h
 * @brief LED control middleware for the RP2350 project.
 */

/**
 * @brief Initialize an LED on a specific GPIO pin.
 * @param pin The GPIO pin number.
 */
void mw_led_init(uint32_t pin);

/**
 * @brief Turn an LED on.
 * @param pin The GPIO pin number.
 */
void mw_led_on(uint32_t pin);

/**
 * @brief Turn an LED off.
 * @param pin The GPIO pin number.
 */
void mw_led_off(uint32_t pin);

/**
 * @brief Toggle an LED.
 * @param pin The GPIO pin number.
 */
void mw_led_toggle(uint32_t pin);

#endif /* MW_LED_H */
