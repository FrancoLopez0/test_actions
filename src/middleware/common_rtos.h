#ifndef COMMON_RTOS_H
#define COMMON_RTOS_H

#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef SemaphoreHandle_t rtos_mutex_t;
typedef SemaphoreHandle_t rtos_sem_t;
typedef QueueHandle_t     rtos_queue_t;

// Mutex
rtos_mutex_t rtos_mutex_create(void);
bool rtos_mutex_lock(rtos_mutex_t mutex, uint32_t timeout_ms);
void rtos_mutex_unlock(rtos_mutex_t mutex);

// Binary Semaphore
rtos_sem_t rtos_sem_create_binary(void);
bool rtos_sem_take(rtos_sem_t sem, uint32_t timeout_ms);
void rtos_sem_give(rtos_sem_t sem);

// Queue
rtos_queue_t rtos_queue_create(uint32_t length, uint32_t item_size);
bool rtos_queue_send(rtos_queue_t queue, const void *item, uint32_t timeout_ms);
bool rtos_queue_receive(rtos_queue_t queue, void *item, uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif

#endif // COMMON_RTOS_H
