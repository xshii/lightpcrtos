/**
 * @file hal_types.h
 * @brief Common types for Hardware Abstraction Layer
 */

#ifndef HAL_TYPES_H
#define HAL_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Return codes */
#define HAL_OK 0
#define HAL_ERROR -1
#define HAL_TIMEOUT -2
#define HAL_BUSY -3

/* Common types */
typedef enum { HAL_STATE_RESET = 0, HAL_STATE_READY, HAL_STATE_BUSY, HAL_STATE_ERROR } HalState;

#endif /* HAL_TYPES_H */
