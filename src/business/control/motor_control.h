/**
 * @file motor_control.h
 * @brief Motor Control Module
 */

#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include "hal_dma.h"
#include "hal_scheduler.h"
#include "hal_types.h"

/* Motor control mode */
typedef enum { MOTOR_MODE_SPEED = 0, MOTOR_MODE_POSITION = 1, MOTOR_MODE_TORQUE = 2 } MotorMode;

/* Motor status */
typedef struct {
    float currentSpeed;
    float targetSpeed;
    float position;
    bool isRunning;
} MotorStatus;

/**
 * @brief Initialize motor control
 * @param dmaId DMA instance ID for data transfer
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int MOTOR_CONTROL_Init(DmaId dmaId);

/**
 * @brief Deinitialize motor control
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int MOTOR_CONTROL_Deinit(void);

/**
 * @brief Set motor mode
 * @param mode Motor control mode
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int MOTOR_CONTROL_SetMode(MotorMode mode);

/**
 * @brief Set target speed
 * @param speed Target speed (RPM)
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int MOTOR_CONTROL_SetSpeed(float speed);

/**
 * @brief Get motor status
 * @param status Output motor status
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int MOTOR_CONTROL_GetStatus(MotorStatus* status);

/**
 * @brief Start motor
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int MOTOR_CONTROL_Start(void);

/**
 * @brief Stop motor
 * @return HAL_OK on success, HAL_ERROR on failure
 */
int MOTOR_CONTROL_Stop(void);

#endif /* MOTOR_CONTROL_H */
