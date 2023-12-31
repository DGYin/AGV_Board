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



/* SYSTEM Settings, DONT CHANGE EASILY! --------------------------------------*/
#define PROTOCOL_POSITION_LSBS		8191	// 统一的角度分辨率。从零点开始CW方向，将角度等分为 PROTOCOL_POSITION_LSBS 份。
#define MAXIMUM_STEERING_HANDLE_NUM	4

/* Includes ------------------------------------------------------------------*/

/** @addtogroup stdint
  * @{
  */
#if defined(STM32F105) | defined(STM32F407)
	#include <stdint.h>
#endif

#if defined(ARM_MATH_CM3) & ( __FPU_PRESENT==1)
	#include "arm_math.h"
#endif

#include "pid_regulator.h"

#if defined(DIRECTIVE_MOTOR_M3508) | defined(MOTION_MOTOR_M3508)
	#include "M3508_gear.h"
#endif

#if defined(DIRECTIVE_ENCODER_BRITER_ENCODER)
	#define  DIRECTIVE_WITH_ENCODER
	#include "briter_encoder.h"
#endif

typedef enum
{
	STEERING_WHEEL_OK,
	STEERING_WHEEL_ERROR,
	STEERING_WHEEL_WRONG_PARAM,
	STEERING_WHEEL_ILLEGAL_HANDLE
} STEERING_WHEEL_RETURN_T;

typedef enum
{
	STEERING_WHEEL_SLEEP,
	STEERING_WHEEL_ACTIVATED,
}STEERING_WHEEL_ENABLE_T;

typedef enum
{
	ENABLE_MINOR_ACR_OPTIMIZEATION,
	DISABLE_MINOR_ACR_OPTIMIZEATION
}STEERING_WHEEL_ARC_OPTIMIZATION_T;

typedef enum
{
	DIRECTION_INVERSE,
	DIRECTION_SAME
}ENCODER_DIRECTIVE_PART_DIRECTION_t;
typedef struct
{
	int32_t speed;
	int32_t position;
	int32_t torque;
	uint32_t get_tick;
	uint32_t last_get_tick;
} steering_wheel_motor_feedback_t, steering_wheel_motor_status_t, steering_wheel_motor_command_t;

typedef struct
{
	uint32_t	lsbs_per_encoder_round;
	uint8_t		encoder_rounds_per_part_round;
	uint32_t	lsbs_per_part_round;
	ENCODER_DIRECTIVE_PART_DIRECTION_t		encoder_directive_part_direction;
}steering_wheel_encoder_parameter_t;
typedef struct
{
	int16_t protocol_position;
	int16_t protocol_speed;
} steering_wheel_directive_part_command_t, steering_wheel_directive_part_status_t, steering_wheel_directive_part_feedback_t;

typedef struct
{
	int16_t protocol_speed;
} steering_wheel_motion_part_command_t, steering_wheel_motion_part_status_t, steering_wheel_motion_part_feedback_t;

typedef struct
{
	uint8_t CANID;
	STEERING_WHEEL_ENABLE_T				enable;
	STEERING_WHEEL_ARC_OPTIMIZATION_T	arc_optimization;
}steering_wheel_parameter_t;

typedef struct
{
	int16_t protocol_position;
	int16_t protocol_speed;
} steering_wheel_command_t;
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
		M3508_gear_t M3508_kit;
	#endif
}steering_wheel_motor_t;

typedef struct
{
	steering_wheel_encoder_parameter_t	parameter;
	steering_wheel_motor_status_t		status;
	steering_wheel_motor_command_t		command;
	steering_wheel_motor_feedback_t		feedback;
	#if defined(DIRECTIVE_ENCODER_BRITER_ENCODER)
		briter_encoder_t briter_encoder;
	#endif
}steering_wheel_encoder_t;

typedef struct
{
	steering_wheel_directive_part_command_t	command;
	steering_wheel_directive_part_status_t	status;
	steering_wheel_motor_t		motor;
	steering_wheel_encoder_t	encoder;
} directive_part_t;

typedef struct
{
	steering_wheel_motion_part_command_t	command;
	steering_wheel_motion_part_status_t		status;
	steering_wheel_motor_t motor;
} motion_part_t;

typedef struct
{
	steering_wheel_command_t	command;
	steering_wheel_parameter_t	parameter;
	directive_part_t	directive_part;
	motion_part_t		motion_part;
}steering_wheel_t;

typedef struct
{
	steering_wheel_t	*handle;
	uint8_t				CANID;
}steering_handle_list_t;

STEERING_WHEEL_RETURN_T Steering_Wheel_MotorFeedbackUpdate(steering_wheel_t *steering_wheel);

STEERING_WHEEL_RETURN_T Steering_Wheel_MotorCommandUpdate(steering_wheel_t *steering_wheel);
STEERING_WHEEL_RETURN_T Steering_Wheel_PartCommandUpdate(steering_wheel_t *steering_wheel);
STEERING_WHEEL_RETURN_T Steering_Wheel_CommandUpdate(steering_wheel_t *steering_wheel);

STEERING_WHEEL_RETURN_T Steering_Wheel_StatusUpdate(steering_wheel_t *steering_wheel);

STEERING_WHEEL_RETURN_T Steering_Wheel_SetProtocolPosition(steering_wheel_t *steering_wheel, uint16_t protocol_position);
STEERING_WHEEL_RETURN_T Steering_Wheel_SetProtocolSpeed(steering_wheel_t *steering_wheel, uint16_t protocol_speed);

STEERING_WHEEL_RETURN_T Steering_Wheel_HandleInit(steering_wheel_t *steering_wheel);
STEERING_WHEEL_RETURN_T Steering_Wheel_CommandTransmit(steering_wheel_t *steering_wheel);

steering_wheel_t *Steering_FindSteeringHandle_via_CANID(uint8_t CANID);


/* CRITICAL Settings, NEVER CHANGE! ------------------------------------------*/
#define STEERING_ILLEGAL_HANDLE (steering_wheel_t*)NULL

#ifdef __cplusplus
}
#endif
#endif /*STEERING_WHEEL_H*/
