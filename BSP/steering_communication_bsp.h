/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STEERING_COMMUNICATION_BSP_H
#define STEERING_COMMUNICATION_BSP_H

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
	OFFSET_DIRECTIVE_ANGLE_LOOP,
	OFFSET_DIRECTIVE_SPEED_LOOP,
	OFFSET_MOTION_ANGLE_LOOP,
	OFFSET_MOTION_SPEED_LOOP,
}GET_PID_PARAMETER_GAIN_LOOP_OFFSET_ID;

typedef enum
{
	OFFSET_DIRECTIVE_ANGLE_LOOP,
	OFFSET_DIRECTIVE_SPEED_LOOP,
	OFFSET_MOTION_ANGLE_LOOP,
	OFFSET_MOTION_SPEED_LOOP,
}GET_PID_PARAMETER_GAIN_TERM_OFFSET_ID;

#ifdef __cplusplus
}
#endif
#endif