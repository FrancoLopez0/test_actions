#ifndef APP_BLINKY_H
#define APP_BLINKY_H
#include "pico/stdlib.h"
#include "app_blinky.h"
#include "mw_led.h"
#include "FreeRTOS.h"
#include "task.h"
/**
 * @file app_blinky.h
 * @brief Blinky application logic.
 */

/**
 * @brief FreeRTOS task that blinks an LED.
 * @param pvParameters Pointer to task parameters (unused).
 */
void app_blinky_task(void *pvParameters);

#endif /* APP_BLINKY_H */
