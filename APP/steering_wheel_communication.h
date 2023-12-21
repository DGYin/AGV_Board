
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STEERING_COMMUNICATION_H
#define STEERING_COMMUNICATION_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/** @addtogroup stdint
  * @{
  */
#include <stdint.h>

typedef enum
{
	STEERING_COMMUNICATION_OK,
	STEERING_COMMUNICATION_ERROR
}STEERING_COMMUNICATION_RETURN_T;

typedef enum
{
	// 默认强制停止运行
	EMERGENCY_SHUTDOWN,
	// 获取 PID 参数值
	GET_PID_PARAMETER_PI_GAIN_DIRECTIVE_ANGLE_RING,
	
	GET_MOTION_PART_PID_PARAMETER_LIMIT,
	// 设置 PID 参数值
	SET_PID_PARAMETER_PI_GAIN_DIRECTIVE_ANGLE_RING,
	SET_PID_PARAMETER_PI_GAIN_DIRECTIVE_SPEED_RING,
	SET_PID_PARAMETER_PI_GAIN_MOTION_ANGLE_RING,
	SET_PID_PARAMETER_PI_GAIN_MOTION_SPEED_RING,
	SET_PID_PARAMETER_PD_GAIN_DIRECTIVE_ANGLE_RING,
	SET_PID_PARAMETER_PD_GAIN_DIRECTIVE_SPEED_RING,
	SET_PID_PARAMETER_PD_GAIN_MOTION_ANGLE_RING,
	SET_PID_PARAMETER_PD_GAIN_MOTION_SPEED_RING,
	
	
	SET_PID_PARAMETER_PI_GAIN,
	
	
}steering_communication_command_id_t;




#ifdef __cplusplus
}
#endif
#endif