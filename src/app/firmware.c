#include "FreeRTOS.h"
#include "task.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "app_blinky.h"

// Task handle for the blinky task
static TaskHandle_t xBlinkyTaskHandle = NULL;

/**
 * @brief Entry point for the second core (Core 1).
 */
void main_core1(void) {
    // Start the FreeRTOS scheduler on Core 1
    vTaskStartScheduler();
}

/**
 * @brief Main firmware entry point that initializes tasks and starts the scheduler.
 */
void start_firmware(void) {
    // Initialize standard I/O for debugging and communication
    stdio_init_all();

    // Create the Blinky task
    // This task will alternate the state of the default onboard LED
    xTaskCreate(
        app_blinky_task,    // Task function
        "BlinkyTask",       // Task name
        1024,               // Stack size (in words)
        NULL,               // Task parameters
        1,                  // Task priority
        &xBlinkyTaskHandle  // Task handle
    );

    // Demonstrate Symmetric Multi-Processing (SMP) affinity by pinning 
    // the blinky task to Core 0. Other unpinned tasks can run on any core.
    #if (configUSE_CORE_AFFINITY == 1)
    vTaskCoreAffinitySet(xBlinkyTaskHandle, (1 << 0));
    #endif

    // Launch Core 1 using the Pico SDK's multicore functions.
    // Core 1 will then start its own instance of the FreeRTOS scheduler.
    multicore_launch_core1(main_core1);

    // Start the FreeRTOS scheduler on Core 0
    vTaskStartScheduler();
}
