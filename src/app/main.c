#include <stdio.h>
#include "pico/stdlib.h"
#include "hal_gpio.h"

#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h"
#define HAS_CYW43 1
#endif

// Extern declaration of the firmware start function
extern void start_firmware(void);

int main() {
    // Standard I/O initialization
    stdio_init_all();

    // Force LED ON if board is Pico W/2W
#if HAS_CYW43
    hal_gpio_init(CYW43_WL_GPIO_LED_PIN, HAL_GPIO_DIR_OUT);
    hal_gpio_write(CYW43_WL_GPIO_LED_PIN, true);
#else
    hal_gpio_init(25, HAL_GPIO_DIR_OUT);
    hal_gpio_write(25, true);
#endif

    // Run firmware
    start_firmware();

    while (true) {
        tight_loop_contents();
    }
}
