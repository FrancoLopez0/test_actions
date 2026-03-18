#include "hal_gpio.h"
#include "pico/stdlib.h"

// The Pico SDK defines CYW43_WL_GPIO_LED_PIN for boards with a CYW43 chip (Pico W/2W)
#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h"
#define HAS_CYW43 1
#endif

void hal_gpio_init(uint32_t pin, hal_gpio_dir_t dir) {
#if HAS_CYW43
    if (pin == CYW43_WL_GPIO_LED_PIN) {
        static bool initialized = false;
        if (!initialized) {
            if (cyw43_arch_init()) return;
            initialized = true;
        }
        return;
    }
#endif
    gpio_init(pin);
    gpio_set_dir(pin, dir == HAL_GPIO_DIR_OUT ? GPIO_OUT : GPIO_IN);
}

void hal_gpio_write(uint32_t pin, bool value) {
#if HAS_CYW43
    if (pin == CYW43_WL_GPIO_LED_PIN) {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, value);
        return;
    }
#endif
    gpio_put(pin, value);
}

bool hal_gpio_read(uint32_t pin) {
#if HAS_CYW43
    if (pin == CYW43_WL_GPIO_LED_PIN) {
        return cyw43_arch_gpio_get(CYW43_WL_GPIO_LED_PIN);
    }
#endif
    return gpio_get(pin);
}

void hal_gpio_toggle(uint32_t pin) {
#if HAS_CYW43
    if (pin == CYW43_WL_GPIO_LED_PIN) {
        bool current = cyw43_arch_gpio_get(CYW43_WL_GPIO_LED_PIN);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, !current);
        return;
    }
#endif
    bool current = gpio_get(pin);
    gpio_put(pin, !current);
}
