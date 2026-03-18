#include "unity.h"
#include "mw_led.h"
#include <stdbool.h>

/* Mock hal_gpio functions */
static uint32_t last_pin_initialized = 0xFFFFFFFF;
static bool last_pin_value = false;
static uint32_t toggle_count = 0;

void hal_gpio_init(uint32_t pin, hal_gpio_dir_t dir) {
    last_pin_initialized = pin;
}

void hal_gpio_write(uint32_t pin, bool value) {
    last_pin_value = value;
}

bool hal_gpio_read(uint32_t pin) {
    return last_pin_value;
}

void hal_gpio_toggle(uint32_t pin) {
    last_pin_value = !last_pin_value;
    toggle_count++;
}

void setUp(void) {
    last_pin_initialized = 0xFFFFFFFF;
    last_pin_value = false;
    toggle_count = 0;
}

void tearDown(void) {
}

void test_mw_led_init(void) {
    uint32_t pin = 25;
    mw_led_init(pin);
    TEST_ASSERT_EQUAL(pin, last_pin_initialized);
}

void test_mw_led_toggle(void) {
    uint32_t pin = 25;
    mw_led_toggle(pin);
    TEST_ASSERT_EQUAL(1, toggle_count);
    TEST_ASSERT_TRUE(last_pin_value);
    
    mw_led_toggle(pin);
    TEST_ASSERT_EQUAL(2, toggle_count);
    TEST_ASSERT_FALSE(last_pin_value);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_mw_led_init);
    RUN_TEST(test_mw_led_toggle);
    return UNITY_END();
}
