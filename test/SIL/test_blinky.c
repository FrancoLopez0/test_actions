// 1. Forzamos la macro para que hal_common.h ignore a pico/stdlib.h
#define SIL_TESTING 

#include "unity.h"
#include "mw_led.h"
#include <stdbool.h>
#include "mock_hal_gpio.h" // CMock genera toda la lógica de rastreo aquí

void setUp(void) {
}

void tearDown(void) {
}

void test_mw_led_init(void) {
    uint32_t pin = 25;
    
    hal_gpio_init_Expect(pin, HAL_GPIO_DIR_OUT); 

    mw_led_init(pin);
}

void test_mw_led_toggle(void) {
    uint32_t pin = 25;
    
    hal_gpio_toggle_Expect(pin);
    mw_led_toggle(pin); 
    
    hal_gpio_toggle_Expect(pin);
    mw_led_toggle(pin); 
}

