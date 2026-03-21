#ifndef I2C_SAFE_H
#define I2C_SAFE_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "middleware/common_rtos.h"
#include "middleware/logging.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    I2C_TYPE_WRITE,
    I2C_TYPE_READ
} i2c_req_type_t;

typedef struct {
    i2c_inst_t *inst;
    uint8_t addr;
    uint8_t *buffer;
    size_t len;
    i2c_req_type_t type;
    rtos_sem_t done_sem;
    int *result; // Stores the return value of SDK call (bytes count or error)
} i2c_request_t;

/**
 * @brief Initializes the I2C safe dispatcher system.
 * 
 * Must be called before any i2c_safe_xxx calls.
 */
void i2c_safe_init(void);

/**
 * @brief Thread-safe synchronous I2C write.
 */
int i2c_safe_write_sync(i2c_inst_t *i2c, uint8_t addr, const uint8_t *src, size_t len, bool nostop);

/**
 * @brief Thread-safe synchronous I2C read.
 */
int i2c_safe_read_sync(i2c_inst_t *i2c, uint8_t addr, uint8_t *dst, size_t len, bool nostop);

#ifdef __cplusplus
}
#endif

#endif // I2C_SAFE_H
