#include <stdio.h>
#include "pico/stdlib.h"
#include "app_blinky.h"
#include "mw_led.h"
#include "FreeRTOS.h"
#include "task.h"

// Define LED pin based on the model detected by the SDK
#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h"
#define BLINKY_LED_PIN CYW43_WL_GPIO_LED_PIN
#else
#define BLINKY_LED_PIN 25 // Standard Pico 2
#endif

void app_blinky_task(void *pvParameters) {
    (void)pvParameters;
    
    mw_led_init(BLINKY_LED_PIN);
    mw_led_off(BLINKY_LED_PIN);
    
    // Stabilize CYW43
    vTaskDelay(pdMS_TO_TICKS(100));
    
    while (1) {
        mw_led_toggle(BLINKY_LED_PIN);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
