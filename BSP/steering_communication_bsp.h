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

/* Public typedef ------------------------------------------------------------*/
typedef uint8_t (*steering_communication_tx_ptr)(void *, uint32_t , uint8_t[]); // handle, extended frame, data frame
typedef uint8_t (*steering_communication_rx_ptr)(void *, uint32_t , uint8_t[]); // handle, extended frame, data frame



typedef enum
{
	OFFSET_DIRECTIVE_POSITION_LOOP = 0x01,
	OFFSET_DIRECTIVE_VELOCITY_LOOP,
	OFFSET_MOTION_POSITION_LOOP,
	OFFSET_MOTION_VELOCITY_LOOP
}GET_PID_PARAMETER_LOOP_OFFSET_ID;

typedef enum
{
	OFFSET_OUTPUT_TERM = 0x01,
	OFFSET_PROPOSITIONAL_TERM,
	OFFSET_INTEGRAL_TERM,
	OFFSET_DIFFERENTIAL_TERM
}GET_PID_PARAMETER_LIMIT_OFFSET_ID;

typedef struct
{
	/** essential  **/
	steering_communication_tx_ptr	tx_cmd;
	
	/** customizable  **/
	void *handle;
} steering_communication_ctx_t;

typedef struct
{
	uint8_t		treated_flag; // 判断本数据包是否已经发送或接收。
	uint8_t		cmd_id;
	uint16_t	data2;
	uint8_t		steering_id;
	int8_t		data1[8];
}steering_communication_pack_t;



uint8_t steering_communication_transmit(steering_communication_ctx_t *ctx, steering_communication_pack_t *pack);
steering_communication_pack_t steering_communication_receive_unpack(uint32_t extid, uint8_t *data1);

#ifdef __cplusplus
}
#endif
#endif
