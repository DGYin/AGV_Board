
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STEERING_WHEEL_COMMUNICATION_H
#define STEERING_WHEEL_COMMUNICATION_H

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
	GET_PID_PARAMETER = 0x30,
	
	
}steering_wheel_communication_command_id_t;




#ifdef __cplusplus
}
#endif
#endif