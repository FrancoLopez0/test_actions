#include "unity.h"
#include "hal_gpio.h"
#include "pico/stdlib.h"

/**
 * @file test_hw_gpio.c
 * @brief HIL test for RP2350 HAL GPIO.
 */

void setUp(void) {
}

void tearDown(void) {
}

/**
 * @brief Test internal GPIO state (Output).
 */
void test_hal_gpio_output(void) {
    const uint32_t test_pin = 2; // Any unused GPIO
    hal_gpio_init(test_pin, HAL_GPIO_DIR_OUT);
    
    hal_gpio_write(test_pin, true);
    TEST_ASSERT_TRUE(hal_gpio_read(test_pin));
    
    hal_gpio_write(test_pin, false);
    TEST_ASSERT_FALSE(hal_gpio_read(test_pin));
}

/**
 * @brief Test GPIO toggling on hardware.
 */
void test_hal_gpio_toggle_hw(void) {
    const uint32_t test_pin = 3;
    hal_gpio_init(test_pin, HAL_GPIO_DIR_OUT);
    
    hal_gpio_write(test_pin, false);
    hal_gpio_toggle(test_pin);
    TEST_ASSERT_TRUE(hal_gpio_read(test_pin));
    
    hal_gpio_toggle(test_pin);
    TEST_ASSERT_FALSE(hal_gpio_read(test_pin));
}

int main(void) {
    // Standard Pico SDK setup - MUST BE CALLED FIRST
    stdio_init_all();
    
    // Wait for serial monitor connection
    sleep_ms(3000);
    
    UNITY_BEGIN();
    RUN_TEST(test_hal_gpio_output);
    RUN_TEST(test_hal_gpio_toggle_hw);
    return UNITY_END();
}
