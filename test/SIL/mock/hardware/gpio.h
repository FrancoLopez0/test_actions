#ifndef MOCK_HARDWARE_GPIO_H
#define MOCK_HARDWARE_GPIO_H

#include <stdint.h>
#include <stdbool.h>

#define GPIO_IN  0
#define GPIO_OUT 1

typedef unsigned int uint;

void gpio_init(uint gpio);
void gpio_set_dir(uint gpio, bool out);
void gpio_put(uint gpio, bool value);
bool gpio_get(uint gpio);

#endif // MOCK_HARDWARE_GPIO_H
