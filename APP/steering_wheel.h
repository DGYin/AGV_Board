/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STEERING_WHEEL_H
#define STEERING_WHEEL_H

#ifdef __cplusplus
extern "C" {
#endif
	
/* USER Settings -------------------------------------------------------------*/
#define STM32F105
#define DIRECTIVE_MOTOR_M3508
#define DIRECTIVE_ENCODER_BRITER_ENCODER
#define MOTION_MOTOR_M3508

/* Includes ------------------------------------------------------------------*/

/** @addtogroup stdint
  * @{
  */
#if defined(STM32F105) | defined(STM32F407)
	#include <stdint.h>
#endif

#include "pid_regulator.h"

#if defined(DIRECTIVE_MOTOR_M3508) | defined(MOTION_MOTOR_M3508)
	#include "M3508_gear.h"
#endif

#if defined(DIRECTIVE_ENCODER_BRITER_ENCODER)
	#include "briter_encoder.h"
#endif

typedef enum
{
	STEERING_WHEEL_OK,
} STEERING_WHEEL_RETURN_T;

typedef struct
{
	uint32_t speed;
	uint32_t position;
	uint32_t torque;
} steering_wheel_motor_feedback_t;

typedef struct
{
	uint32_t speed;
	uint32_t position;
	uint32_t torque;
} steering_wheel_motor_status_t;

typedef struct
{
	uint32_t speed;
	uint32_t position;
	uint32_t torque;
} steering_wheel_motor_command_t;



typedef struct
{
	PID_Handle_t position_loop_handle;
	PID_Handle_t velocity_loop_handle;
}steering_wheel_pid_handles_t;

typedef struct
{
	steering_wheel_pid_handles_t 	PID_Handles;
	steering_wheel_motor_status_t	status;
	steering_wheel_motor_command_t	command;
	steering_wheel_motor_feedback_t	feedback;
	#if defined(DIRECTIVE_MOTOR_M3508) | defined(MOTION_MOTOR_M3508)
		M3508_gear_t M3508_motor;
	#endif
}steering_wheel_motor_t;

typedef struct
{
	steering_wheel_motor_status_t	status;
	steering_wheel_motor_command_t	command;
	steering_wheel_motor_feedback_t	feedback;
	#if defined(DIRECTIVE_ENCODER_BRITER_ENCODER)
		briter_encoder_t briter_encoder;
	#endif
}steering_wheel_encoder_t;

typedef struct 
{
	
	steering_wheel_motor_t		direction_motor;
	steering_wheel_encoder_t	encoder;
} directive_part_t;

typedef struct 
{
	steering_wheel_motor_t motion_motor;
} motion_part_t;

typedef struct
{
	
	directive_part_t	directive_part;
	motion_part_t		motion_part;
}steering_wheel_t;


void Steering_Wheel_PidHandleCreate(void);
STEERING_WHEEL_RETURN_T Steering_Wheel_RawDataUpdate(steering_wheel_t *steering_wheel);




#ifdef __cplusplus
}
#endif
#endif /*STEERING_WHEEL_H*/
