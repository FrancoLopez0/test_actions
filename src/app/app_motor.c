#include "app_motor.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "mw_led.h"
#include "FreeRTOS.h"
#include "task.h"
#include "motor.h"

#define PIN_PWM         16
#define PWM_FREQ        15000
#define CLK_DIV         1
#define PWM_WRAP        3750
//----Motor A---
#define PIN_A_1         22
#define PIN_A_2         20

motor_t motor_a = {
    .dir = CLOCKWISE,
    .duty_cycle = 50.0f,
};

motor_config_t motor_conf = {
    .pin_a = PIN_A_1,
    .pin_b = PIN_A_2,
    .pin_pwm = PIN_PWM,
    .frequency_hz = PWM_FREQ,
};

void task_motor(void *params){
    vTaskDelay(pdMS_TO_TICKS(5000));
    motor_config(&motor_a, &motor_conf);

    while (true){
        vTaskDelay(pdMS_TO_TICKS(1000));
        motor_set_lvl(&motor_a, 50);
        vTaskDelay(pdMS_TO_TICKS(1000));
        motor_set_lvl(&motor_a, 90);
    }
    
}