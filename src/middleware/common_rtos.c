#include "middleware/common_rtos.h"

rtos_mutex_t rtos_mutex_create(void) {
    return xSemaphoreCreateMutex();
}

bool rtos_mutex_lock(rtos_mutex_t mutex, uint32_t timeout_ms) {
    if (mutex == NULL) return false;
    return xSemaphoreTake(mutex, pdMS_TO_TICKS(timeout_ms)) == pdTRUE;
}

void rtos_mutex_unlock(rtos_mutex_t mutex) {
    if (mutex != NULL) {
        xSemaphoreGive(mutex);
    }
}

rtos_sem_t rtos_sem_create_binary(void) {
    return xSemaphoreCreateBinary();
}

bool rtos_sem_take(rtos_sem_t sem, uint32_t timeout_ms) {
    if (sem == NULL) return false;
    return xSemaphoreTake(sem, pdMS_TO_TICKS(timeout_ms)) == pdTRUE;
}

void rtos_sem_give(rtos_sem_t sem) {
    if (sem != NULL) {
        xSemaphoreGive(sem);
    }
}

rtos_queue_t rtos_queue_create(uint32_t length, uint32_t item_size) {
    return xQueueCreate(length, item_size);
}

bool rtos_queue_send(rtos_queue_t queue, const void *item, uint32_t timeout_ms) {
    if (queue == NULL) return false;
    return xQueueSend(queue, item, pdMS_TO_TICKS(timeout_ms)) == pdPASS;
}

bool rtos_queue_receive(rtos_queue_t queue, void *item, uint32_t timeout_ms) {
    if (queue == NULL) return false;
    return xQueueReceive(queue, item, pdMS_TO_TICKS(timeout_ms)) == pdPASS;
}
