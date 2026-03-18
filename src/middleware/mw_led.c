#include "mw_led.h"

void mw_led_init(uint32_t pin) {
    hal_gpio_init(pin, HAL_GPIO_DIR_OUT);
}

void mw_led_on(uint32_t pin) {
    hal_gpio_write(pin, true);
}

void mw_led_off(uint32_t pin) {
    hal_gpio_write(pin, false);
}

void mw_led_toggle(uint32_t pin) {
    hal_gpio_toggle(pin);
}
