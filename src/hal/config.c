#include "hal/config.h"

__attribute__((weak)) void pwm_irq(){
    int slice_num = pwm_gpio_to_slice_num(PIN_PWM);
    pwm_clear_irq(slice_num);
    gpio_put(PIN_IRQ_CHECK, !gpio_get(PIN_IRQ_CHECK));
    pwm_set_irq_enabled(slice_num, true);
}

void config_gpio(){
    gpio_init(PIN_IRQ_CHECK);

    gpio_set_dir(PIN_IRQ_CHECK, GPIO_OUT);

    gpio_set_function(PIN_PWM, GPIO_FUNC_PWM);

    gpio_init(PIN_LED);
    gpio_set_dir(PIN_LED, GPIO_OUT);
}

void config_pwm(){
    uint slice_num = pwm_gpio_to_slice_num(PIN_PWM);

    pwm_config config = pwm_get_default_config();

    pwm_config_set_wrap(&config, PWM_WRAP);

    pwm_config_set_phase_correct(&config, true);

    pwm_init(slice_num, &config, true);

    pwm_set_gpio_level(PIN_PWM, 0);

    pwm_clear_irq(slice_num);
    
    pwm_set_irq_enabled(slice_num, true);
    
    irq_set_exclusive_handler(PWM_DEFAULT_IRQ_NUM(), pwm_irq);

    irq_set_enabled(PWM_DEFAULT_IRQ_NUM(), true);
}

void _config(){
    config_gpio();
    config_pwm();
}