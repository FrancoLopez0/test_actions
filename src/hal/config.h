#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "hardware/adc.h"
#include "hardware/pwm.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/irq.h"

#include "board_config.h"

void pwm_irq();

void _config();

#ifdef __cplusplus
}
#endif

#endif // CONFIG_H