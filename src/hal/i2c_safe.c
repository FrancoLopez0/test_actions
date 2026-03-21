#include "hal/i2c_safe.h"
#include "FreeRTOS.h"
#include "task.h"

#define I2C_SAFE_TAG "I2C_SAFE"
#define REQUEST_QUEUE_LEN 10

static rtos_queue_t xI2CRequestQueue = NULL;

static void i2c_dispatcher_task(void *arg) {
    i2c_request_t req;

    LOGI(I2C_SAFE_TAG, "Dispatcher task started");

    while (true) {
        // Wait for a request
        if (rtos_queue_receive(xI2CRequestQueue, &req, portMAX_DELAY)) {
            int ret = 0;

            // Execute the I2C operation
            if (req.type == I2C_TYPE_WRITE) {
                ret = i2c_write_blocking(req.inst, req.addr, req.buffer, req.len, false);
            } else if (req.type == I2C_TYPE_READ) {
                ret = i2c_read_blocking(req.inst, req.addr, req.buffer, req.len, false);
            }

            // Store result and signal completion
            if (req.result != NULL) {
                *(req.result) = ret;
            }
            rtos_sem_give(req.done_sem);
        }
    }
}

void i2c_safe_init(void) {
    if (xI2CRequestQueue != NULL) return; // Already initialized

    xI2CRequestQueue = rtos_queue_create(REQUEST_QUEUE_LEN, sizeof(i2c_request_t));
    if (xI2CRequestQueue == NULL) {
        LOGE(I2C_SAFE_TAG, "Failed to create request queue");
        return;
    }

    xTaskCreate(
        i2c_dispatcher_task,
        "I2C_DISPATCH",
        configMINIMAL_STACK_SIZE * 4,
        NULL,
        tskIDLE_PRIORITY + 2,
        NULL
    );
}

int i2c_safe_write_sync(i2c_inst_t *i2c, uint8_t addr, const uint8_t *src, size_t len, bool nostop) {
    int result = 0;
    rtos_sem_t done_sem = rtos_sem_create_binary();
    if (done_sem == NULL) return -1;

    i2c_request_t req = {
        .inst = i2c,
        .addr = addr,
        .buffer = (uint8_t *)src,
        .len = len,
        .type = I2C_TYPE_WRITE,
        .done_sem = done_sem,
        .result = &result
    };

    if (rtos_queue_send(xI2CRequestQueue, &req, pdMS_TO_TICKS(100))) {
        // Wait for the dispatcher to complete the job
        if (!rtos_sem_take(done_sem, pdMS_TO_TICKS(500))) {
            LOGE(I2C_SAFE_TAG, "Timeout waiting for I2C write");
            result = -1;
        }
    } else {
        LOGE(I2C_SAFE_TAG, "Failed to enqueue I2C write");
        result = -1;
    }

    vSemaphoreDelete(done_sem);
    return result;
}

int i2c_safe_read_sync(i2c_inst_t *i2c, uint8_t addr, uint8_t *dst, size_t len, bool nostop) {
    int result = 0;
    rtos_sem_t done_sem = rtos_sem_create_binary();
    if (done_sem == NULL) return -1;

    i2c_request_t req = {
        .inst = i2c,
        .addr = addr,
        .buffer = dst,
        .len = len,
        .type = I2C_TYPE_READ,
        .done_sem = done_sem,
        .result = &result
    };

    if (rtos_queue_send(xI2CRequestQueue, &req, pdMS_TO_TICKS(100))) {
        if (!rtos_sem_take(done_sem, pdMS_TO_TICKS(500))) {
            LOGE(I2C_SAFE_TAG, "Timeout waiting for I2C read");
            result = -1;
        }
    } else {
        LOGE(I2C_SAFE_TAG, "Failed to enqueue I2C read");
        result = -1;
    }

    vSemaphoreDelete(done_sem);
    return result;
}
