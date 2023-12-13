/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STEERING_WHEEL_BSP_H
#define STEERING_WHEEL_BSP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Settings ------------------------------------------------------------------*/


/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

typedef enum
{
	SAME_DIRECTION		= 1,
	INVERSE_DIRECTION	=-1,
} DIRECTION_FLAG;

typedef struct
{
	float	reduction_rate_output_devidedby_motor; // 电机齿轮箱齿比
} motor_kit_t;

typedef struct
{
	float			reduction_rate_drive_devidedby_motor_kit; // 传动装置传动比
	DIRECTION_FLAG	direction_flag; // 经过齿传动装置后，航向电机与舵轮转动方向是否相同
} drive_gear_t;
typedef struct
{
	
	uint32_t test;
}steering_wheel_parameter_t;

typedef struct
{
	uint32_t test;
	
} steering_wheel_command_t;

typedef struct
{
	
	uint32_t test;
} steering_wheel_feedback_t;



typedef struct
{
	uint32_t test;
	
}steering_wheel_course_t;

typedef struct
{
	uint32_t test;
} steering_wheel_velocity_t;

typedef struct
{
	steering_wheel_course_t course;
	steering_wheel_velocity_t velocity;
	
}steering_wheel_status_t;


typedef struct
{
	motor_kit_t motor;
	drive_gear_t drive_gear;
} directive_part_t;
	
typedef struct
{
	uint32_t test;
} thrusting_part_t;

typedef struct
{
	steering_wheel_status_t		command;
	steering_wheel_status_t		status;
	steering_wheel_feedback_t	feedback;
	steering_wheel_parameter_t	parameter;
	
	directive_part_t			directive_part;
	thrusting_part_t			thrusting_part;
	
} steering_wheel_t;


#ifdef __cplusplus
}
#endif
#endif