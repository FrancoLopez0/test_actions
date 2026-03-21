#include "unity.h"
#include "pico/stdlib.h"
#include "pico/stdio_usb.h"
#include "hardware/pwm.h"
#include "middleware/motor.h"
#include "middleware/logging.h"
#include "board_config.h"
#include "FreeRTOS.h"
#include "task.h"

//---- Constantes locales para Inducción de Errores y Warnings ----
#define PIN_ERROR_INVALIDO      99
#define PIN_ERROR_RESERVADO     25
#define DIR_ERROR               3
#define SLICE_ERROR             10
#define FREQ_WARNING_MAX        50000  // Mayor a FREQ_MAX (36000)
#define FREQ_WARNING_LOW        10     // Frecuencia muy baja para forzar clk_div > 255
#define FREQ_SUCCESS            20000  // Frecuencia válida para test de éxito puro
#define DUTY_CYCLE_WARNING      120.0f
#define DUTY_CYCLE_SUCCESS      30.0f

void setUp(void) {}
void tearDown(void) {}

// =========================================================================
// Función: motor_set_lvl
// =========================================================================

void test_error_puntero_nulo_set_lvl(void) {
    // Arrange
    float duty = DUTY_CYCLE_SUCCESS;
    // Act & Assert
    TEST_ASSERT_EQUAL_INT(MOTOR_ERROR_PUNTERO_NULO, motor_set_lvl(NULL, duty));
}

void test_success_set_lvl(void) {
    // Arrange
    motor_t test_motor = {0};
    test_motor.motor_config.pin_pwm = PIN_PWM;
    test_motor.pwm_internal_config.wrap_value = PWM_WRAP;
    
    // Act
    motor_error_t res = motor_set_lvl(&test_motor, DUTY_CYCLE_SUCCESS);

    // Assert
    TEST_ASSERT_EQUAL_INT(MOTOR_SUCCESS, res);
    TEST_ASSERT_EQUAL_FLOAT(DUTY_CYCLE_SUCCESS, test_motor.duty_cycle);
}

// =========================================================================
// Función: motor_set_dir
// =========================================================================

void test_error_puntero_nulo_dir(void) {
    // Act & Assert
    TEST_ASSERT_EQUAL_INT(MOTOR_ERROR_PUNTERO_NULO, motor_set_dir(NULL, CLOCKWISE));
}

void test_error_dir_invalida(void) {
    // Arrange
    motor_t test_motor = {0};
    test_motor.motor_config.pin_a = PIN_A_1;
    test_motor.motor_config.pin_b = PIN_A_2;

    // Act
    motor_error_t res = motor_set_dir(&test_motor, (dir_t)DIR_ERROR);

    // Assert
    TEST_ASSERT_EQUAL_INT(MOTOR_ERROR_DIRECCION_INVALIDA, res);
}

void test_success_set_dir(void) {
    // Arrange
    motor_t test_motor = {0};
    test_motor.motor_config.pin_a = PIN_A_1;
    test_motor.motor_config.pin_b = PIN_A_2;

    // Act
    motor_error_t res = motor_set_dir(&test_motor, COUNTERCLOCKWISE);

    // Assert
    TEST_ASSERT_EQUAL_INT(MOTOR_SUCCESS, res);
    TEST_ASSERT_EQUAL_INT(COUNTERCLOCKWISE, test_motor.dir);
}

// =========================================================================
// Función: motor_config
// =========================================================================

void test_error_puntero_nulo_motor_config(void) {
    // Arrange
    motor_config_t config = {0};
    
    // Act & Assert
    TEST_ASSERT_EQUAL_INT(MOTOR_ERROR_PUNTERO_NULO, motor_config(NULL, &config));
}

void test_success_config_motor(void) {
    // Arrange
    motor_t test_motor = {0};
    motor_config_t config = {
        .pin_pwm = PIN_PWM,
        .pin_a = PIN_A_1,
        .pin_b = PIN_A_2
    };
    test_motor.motor_config.frequency_hz = FREQ_SUCCESS;
    test_motor.dir = STOP;

    // Act
    motor_error_t res = motor_config(&test_motor, &config);

    // Assert
    TEST_ASSERT_EQUAL_INT(MOTOR_SUCCESS, res);
    TEST_ASSERT_EQUAL_INT(PIN_PWM, test_motor.motor_config.pin_pwm);
}

// =========================================================================
// Función: config_gpio_pwm
// =========================================================================

void test_error_pin_pwm_invalid(void) {
    // Act & Assert
    TEST_ASSERT_EQUAL_INT(MOTOR_ERROR_PWM_INVALIDO, config_gpio_pwm(PIN_ERROR_INVALIDO));
}

void test_error_pin_pwm_reservado(void) {
    // Act & Assert
    TEST_ASSERT_EQUAL_INT(MOTOR_ERROR_PIN_RESERVADO, config_gpio_pwm(PIN_ERROR_RESERVADO));
}

void test_success_pin_pwm_config(void) {
    // Act & Assert
    TEST_ASSERT_EQUAL_INT(MOTOR_SUCCESS, config_gpio_pwm(PIN_PWM));
}

// =========================================================================
// Función: config_gpio_output
// =========================================================================

void test_error_pin_out_invalid(void) {
    // Act & Assert
    TEST_ASSERT_EQUAL_INT(MOTOR_ERROR_PIN_INVALIDO, config_gpio_output(PIN_ERROR_INVALIDO));
}

void test_error_pin_out_reservado(void) {
    // Act & Assert
    TEST_ASSERT_EQUAL_INT(MOTOR_ERROR_PIN_RESERVADO, config_gpio_output(PIN_ERROR_RESERVADO));
}

void test_success_pin_out_config(void) {
    // Act & Assert
    TEST_ASSERT_EQUAL_INT(MOTOR_SUCCESS, config_gpio_output(PIN_A_1));
}

// =========================================================================
// Función: calculate_pwm_parameters
// =========================================================================

void test_warning_freq_max_calculate_pwm_parameters(void) {
    // Arrange
    pwm_internal_config_t internal = {0};
    uint32_t freq = FREQ_WARNING_MAX;
    // Act
    calculate_pwm_parameters(&freq, &internal);
    
    // Assert
    TEST_ASSERT_EQUAL_UINT32(FREQ_MAX, freq);
}

void test_warning_div_max_calculate_pwm_parameters(void) {
    // Arrange
    pwm_internal_config_t internal = {0};
    uint32_t freq = FREQ_WARNING_LOW;
    // Act
    motor_error_t res = calculate_pwm_parameters(&freq, &internal);
    
    // Assert
    TEST_ASSERT_EQUAL_INT(MOTOR_WARNING_DIV_MAX_SUPERADO, res);
}

void test_success_calculate_pwm_parameters(void) {
    // Arrange
    pwm_internal_config_t internal = {0};
    uint32_t freq = FREQ_SUCCESS;
    // Act
    motor_error_t res = calculate_pwm_parameters(&freq, &internal);
    
    // Assert
    TEST_ASSERT_EQUAL_INT(MOTOR_SUCCESS, res);
    TEST_ASSERT_EQUAL_UINT32(FREQ_SUCCESS, freq);
    TEST_ASSERT_EQUAL_UINT16(WRAP_ESTABLECIDO, internal.wrap_value);
    TEST_ASSERT_TRUE(internal.clk_div >= 1.0f && internal.clk_div <= 255.0f);
}

// =========================================================================
// Función: validate_duty_cycle
// =========================================================================

void test_warning_duty_cycle_superado(void) {
    // Arrange
    float duty = DUTY_CYCLE_WARNING;
    
    // Act
    motor_error_t res = validate_duty_cycle(&duty);
    
    // Assert
    TEST_ASSERT_EQUAL_INT(MOTOR_WARNING_DUTY_CYCLE_SUPERADO, res);
    TEST_ASSERT_EQUAL_FLOAT(100.0f, duty);
}

void test_success_validate_duty_cycle(void) {
    // Arrange
    float duty = DUTY_CYCLE_SUCCESS;
    
    // Act
    motor_error_t res = validate_duty_cycle(&duty);
    
    // Assert
    TEST_ASSERT_EQUAL_INT(MOTOR_SUCCESS, res);
    TEST_ASSERT_EQUAL_FLOAT(DUTY_CYCLE_SUCCESS, duty);
}

// =========================================================================
// Función: pwm_set_config
// =========================================================================

void test_error_puntero_nulo_pwm_set_config(void) {
    // Act & Assert
    TEST_ASSERT_EQUAL_INT(MOTOR_ERROR_PUNTERO_NULO, pwm_set_config(NULL));
}

void test_success_pwm_set_config(void) {
    // Arrange
    motor_t test_motor = {0};
    test_motor.motor_config.pin_pwm = PIN_PWM;
    test_motor.pwm_internal_config.clk_div = CLK_DIV;
    test_motor.pwm_internal_config.wrap_value = PWM_WRAP;
    
    // Hardware dep: aseguramos que el pin está configurado en PWM para obtener un slice válido.
    gpio_set_function(PIN_PWM, GPIO_FUNC_PWM);
    
    // Act
    motor_error_t res = pwm_set_config(&test_motor);
    
    // Assert
    TEST_ASSERT_EQUAL_INT(MOTOR_SUCCESS, res);
    TEST_ASSERT_EQUAL_INT(pwm_gpio_to_slice_num(PIN_PWM), test_motor.pwm_internal_config.slice);
}

// =========================================================================
// Función: motor_pwm_set_enabled
// =========================================================================

void test_error_slice_invalido_motor_pwm_set_enabled(void) {
    // Act & Assert
    TEST_ASSERT_EQUAL_INT(MOTOR_ERROR_SLICE_INVALIDO, motor_pwm_set_enabled(SLICE_ERROR, true));
}

void test_success_motor_pwm_set_enabled(void) {
    // Arrange
    uint8_t slice_valido = 0; // Slice válido genérico (0 a 8)
    
    // Act & Assert
    TEST_ASSERT_EQUAL_INT(MOTOR_SUCCESS, motor_pwm_set_enabled(slice_valido, true));
}

// =========================================================================
// Tarea Runner y Main
// =========================================================================

void runner_task(void *arg) {
    // Inicializar logs DESPUES de que arranque el scheduler
    log_init();

    // Pequeño retraso para que el stack USB se estabilice
    vTaskDelay(pdMS_TO_TICKS(5000));

    UNITY_BEGIN();
    
    // motor_set_lvl
    RUN_TEST(test_error_puntero_nulo_set_lvl);
    RUN_TEST(test_success_set_lvl);

    // motor_set_dir
    RUN_TEST(test_error_puntero_nulo_dir);
    RUN_TEST(test_error_dir_invalida);
    RUN_TEST(test_success_set_dir);

    // motor_config
    RUN_TEST(test_error_puntero_nulo_motor_config);
    RUN_TEST(test_success_config_motor);

    // config_gpio_pwm
    RUN_TEST(test_error_pin_pwm_invalid);
    RUN_TEST(test_error_pin_pwm_reservado);
    RUN_TEST(test_success_pin_pwm_config);

    // config_gpio_output
    RUN_TEST(test_error_pin_out_invalid);
    RUN_TEST(test_error_pin_out_reservado);
    RUN_TEST(test_success_pin_out_config);

    // calculate_pwm_parameters
    RUN_TEST(test_warning_freq_max_calculate_pwm_parameters);
    RUN_TEST(test_warning_div_max_calculate_pwm_parameters);
    RUN_TEST(test_success_calculate_pwm_parameters);

    // validate_duty_cycle
    RUN_TEST(test_warning_duty_cycle_superado);
    RUN_TEST(test_success_validate_duty_cycle);

    // pwm_set_config
    RUN_TEST(test_error_puntero_nulo_pwm_set_config);
    RUN_TEST(test_success_pwm_set_config);

    // motor_pwm_set_enabled
    RUN_TEST(test_error_slice_invalido_motor_pwm_set_enabled);
    RUN_TEST(test_success_motor_pwm_set_enabled);

    UNITY_END();

    printf("\n--- Tests Finished ---\n");

    while(true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int main(void) {
    // Pequeño retraso de hardware antes de inicializar nada
    sleep_ms(5000);
    stdio_init_all();
    printf("XD");
    // Crear la tarea que correra los tests
    xTaskCreate(
        runner_task,
        "RUNNER",
        configMINIMAL_STACK_SIZE * 8,
        NULL,
        tskIDLE_PRIORITY + 1,
        NULL
    );

    // Arrancar el scheduler
    vTaskStartScheduler();

    while(true) {}
    return 0;
}
