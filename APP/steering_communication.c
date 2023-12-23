#include "steering_communication.h"
#include "steering_wheel.h"

#if defined(STM32F105) | (STM32F407)
    #include "can.h"
#endif

/* CAN Bus Settings ----------------------------------------------------------*/
#if defined(STM32F105) | (STM32F407)
    CAN_TxHeaderTypeDef steering_communication_CAN_TxHeaderStruct;
    uint32_t  steering_communication_pTxMailbox;
#endif

// Function prototypes
static uint8_t platform_trans	(void *handle, uint32_t Ext_CANID, uint8_t aData[]);
static uint8_t platform_get_msg	(void *handle, uint32_t Ext_CANID, uint8_t aData[]);
void steering_communication_tx_queue_pop(steering_communication_ctx_t *ctx);	// 发送环形发送队列中的数据
void steering_communication_tx_queue_push(steering_communication_pack_t pack);	// 将待发送数据送入环形发送队列中
uint64_t steering_communication_GET_PID_PARAMETER_term_handler(PID_Handle_t pid_handle, uint8_t term_offset_id);
steering_communication_pack_t steering_communication_GET_PID_PARAMETER_handler(steering_wheel_t *steering, steering_communication_pack_t rx_pack);
void steering_communication_SET_PID_PARAMETER_term_handler(PID_Handle_t *pid_handle, int16_t *rx_byte, uint8_t term_offset_id);
steering_communication_pack_t steering_communication_SET_PID_PARAMETER_handler(steering_wheel_t *steering, steering_communication_pack_t rx_pack);


steering_communication_ctx_t steering_communication_ctx;

void steering_communication_init(void)
{
    /* Initialize transmission functions */
    steering_communication_ctx.tx_cmd = platform_trans;
	steering_communication_ctx.handle = &STEERING_COMMUNICATION_HANDLE;
    /* Initialize CAN driver interface */
    #if defined(STM32F105) | (STM32F407)
		steering_communication_CAN_TxHeaderStruct.StdId = 0;
        steering_communication_CAN_TxHeaderStruct.ExtId = 0;
        steering_communication_CAN_TxHeaderStruct.DLC = 8;
        steering_communication_CAN_TxHeaderStruct.IDE = CAN_ID_EXT; // 使用拓展帧
        steering_communication_CAN_TxHeaderStruct.RTR = CAN_RTR_DATA;
        steering_communication_CAN_TxHeaderStruct.TransmitGlobalTime = DISABLE;
    #endif
}

void steering_communication_test(steering_wheel_t steering)
{
	steering_communication_pack_t test_pack;
	test_pack.cmd_id = 0x14;
	uint64_t testdata1 = 0x1234567890ABCDEF;
	memcpy(&test_pack.data1, &testdata1, sizeof(test_pack.data1));
	test_pack.data2 = 0x0102;
	steering_communication_SET_PID_PARAMETER_handler(&steering, test_pack);
}

STEERING_COMMUNICATION_RETURN_T steering_communication_rx_handler(uint32_t extid, uint8_t data1[])
{
	steering_communication_pack_t pack;
	pack = steering_communication_receive_unpack(extid, data1);
	switch (pack.cmd_id)
	{
		case GET_PID_PARAMETER:
			pack = steering_communication_GET_PID_PARAMETER_handler(, pack);
			break;
		case SET_PID_PARAMETER:
			pack = steering_communication_SET_PID_PARAMETER_handler(, pack);
			break;
		default:
			return STEERING_COMMUNICATION_WRONG_PARAM;
			break;
	}
	steering_communication_transmit(steering_communication_ctx.handle, &pack);
	return STEERING_COMMUNICATION_OK;
}



/*
	用于返回查询某个舵轮的指定 PID 的某环节值
*/
uint64_t steering_communication_GET_PID_PARAMETER_term_handler(PID_Handle_t pid_handle, uint8_t term_offset_id)
{
	uint64_t return_data1 = 0;
	int16_t shortern_UpperIntegralLimit, shortern_LowerIntegralLimit;
	shortern_UpperIntegralLimit = pid_handle.wUpperIntegralLimit; // 原来是32位的，要把它强制换成16位
	shortern_LowerIntegralLimit = pid_handle.wLowerIntegralLimit; // 原来是32位的，要把它强制换成16位
	switch (term_offset_id)
	{
		case OFFSET_OUTPUT_TERM:
			memcpy((uint16_t *)&return_data1,	&pid_handle.hUpperOutputLimit, sizeof(pid_handle.hUpperOutputLimit));
			memcpy((uint16_t *)&return_data1+1,	&pid_handle.hLowerOutputLimit, sizeof(pid_handle.hLowerOutputLimit));
			break;
		case OFFSET_PROPOSITIONAL_TERM:
			memcpy((uint16_t *)&return_data1,	&pid_handle.hKpGain,		sizeof(pid_handle.hKpGain));
			memcpy((uint16_t *)&return_data1+1,	&pid_handle.hKpDivisorPOW2, sizeof(pid_handle.hKpDivisorPOW2));
			break;
		case OFFSET_INTEGRAL_TERM:
			memcpy((uint16_t *)&return_data1,	&pid_handle.hKiGain,			sizeof(pid_handle.hKiGain));
			memcpy((uint16_t *)&return_data1+1,	&pid_handle.hKiDivisorPOW2,		sizeof(pid_handle.hKiDivisorPOW2));
			memcpy((uint16_t *)&return_data1+2,	&shortern_UpperIntegralLimit,	sizeof(shortern_UpperIntegralLimit));
			memcpy((uint16_t *)&return_data1+3,	&shortern_LowerIntegralLimit,	sizeof(shortern_LowerIntegralLimit));
			break;
		case OFFSET_DIFFERENTIAL_TERM:
			memcpy((uint16_t *)&return_data1,	&pid_handle.hKdGain,		sizeof(pid_handle.hKdGain));
			memcpy((uint16_t *)&return_data1+1,	&pid_handle.hKdDivisorPOW2, sizeof(pid_handle.hKdDivisorPOW2));
			break;
		default:
			memset(&return_data1, 0, sizeof(return_data1));
			break;
	
	}
	return return_data1;
}
/*
	用于返回查询某个舵轮的指定某 PID 值
*/
steering_communication_pack_t steering_communication_GET_PID_PARAMETER_handler(steering_wheel_t *steering, steering_communication_pack_t rx_pack)
{
	steering_communication_pack_t return_pack;
	uint8_t term_offset_id, loop_offset_id;
	memcpy(&loop_offset_id, &rx_pack.data2,		1);
	memcpy(&term_offset_id, (uint8_t *)&rx_pack.data2+1,	1);
	memcpy(&return_pack, &rx_pack, sizeof(rx_pack));
	uint64_t processed_data1;
	switch (loop_offset_id)
	{
		case OFFSET_DIRECTIVE_POSITION_LOOP:
			processed_data1 = steering_communication_GET_PID_PARAMETER_term_handler(steering->directive_part.motor.PID_Handles.position_loop_handle, term_offset_id);
			break;
		case OFFSET_DIRECTIVE_VELOCITY_LOOP:
			processed_data1 = steering_communication_GET_PID_PARAMETER_term_handler(steering->directive_part.motor.PID_Handles.velocity_loop_handle, term_offset_id);
			break;
		case OFFSET_MOTION_POSITION_LOOP:
			processed_data1 = steering_communication_GET_PID_PARAMETER_term_handler(steering->motion_part.motor.PID_Handles.position_loop_handle, term_offset_id);
			break;
		case OFFSET_MOTION_VELOCITY_LOOP:
			processed_data1 = steering_communication_GET_PID_PARAMETER_term_handler(steering->motion_part.motor.PID_Handles.velocity_loop_handle, term_offset_id);
			break;
		default:
			break;
	}
	memcpy(&return_pack.data1, &processed_data1, sizeof(processed_data1)); // 除了数据区1的内容，其他原封不动
	return return_pack;
}


/*
	用于设置某个舵轮的指定 PID 的某环节值
*/
void steering_communication_SET_PID_PARAMETER_term_handler(PID_Handle_t *pid_handle, int16_t *rx_byte, uint8_t term_offset_id)
{
	uint64_t return_data1 = 0;
	switch (term_offset_id)
	{
		case OFFSET_OUTPUT_TERM:
			memcpy(&pid_handle->hUpperOutputLimit,	rx_byte,	sizeof(pid_handle->hUpperOutputLimit));
			memcpy(&pid_handle->hLowerOutputLimit,	rx_byte+1,	sizeof(pid_handle->hLowerOutputLimit));
			break;
		case OFFSET_PROPOSITIONAL_TERM:
			memcpy(&pid_handle->hKpGain,		rx_byte,	sizeof(pid_handle->hKpGain));
			memcpy(&pid_handle->hKpDivisorPOW2,	rx_byte+1,	sizeof(pid_handle->hKpDivisorPOW2));
			break;
		case OFFSET_INTEGRAL_TERM:
			memcpy(&pid_handle->hKiGain,				rx_byte,	sizeof(pid_handle->hKiGain));
			memcpy(&pid_handle->hKiDivisorPOW2,			rx_byte+1,	sizeof(pid_handle->hKiDivisorPOW2));
			memcpy(&pid_handle->wUpperIntegralLimit,	rx_byte+2,	sizeof(int16_t));
			memcpy(&pid_handle->wLowerIntegralLimit,	rx_byte+3,	sizeof(int16_t));
			break;
		case OFFSET_DIFFERENTIAL_TERM:
			memcpy(&pid_handle->hKdGain,		rx_byte,	sizeof(pid_handle->hKdGain));
			memcpy(&pid_handle->hKdDivisorPOW2,	rx_byte+1,	sizeof(pid_handle->hKdDivisorPOW2));
			break;
		default:
			memset(&return_data1, 0, sizeof(return_data1));
			break;
	}
}
/*
	用于设置某个舵轮的指定某 PID 值
*/
steering_communication_pack_t steering_communication_SET_PID_PARAMETER_handler(steering_wheel_t *steering, steering_communication_pack_t rx_pack)
{
	uint8_t term_offset_id, loop_offset_id;
	memcpy(&loop_offset_id, &rx_pack.data2,		1);
	memcpy(&term_offset_id, (uint8_t *)&rx_pack.data2+1,	1);
	int16_t rx_data1_byte[4];
	memcpy(&rx_data1_byte, &rx_pack.data1, sizeof(rx_data1_byte)); 
	switch (loop_offset_id)
	{
		case OFFSET_DIRECTIVE_POSITION_LOOP:
			steering_communication_SET_PID_PARAMETER_term_handler(&steering->directive_part.motor.PID_Handles.position_loop_handle, rx_data1_byte, term_offset_id);
			break;
		case OFFSET_DIRECTIVE_VELOCITY_LOOP:
			steering_communication_SET_PID_PARAMETER_term_handler(&steering->directive_part.motor.PID_Handles.velocity_loop_handle, rx_data1_byte, term_offset_id);
			break;
		case OFFSET_MOTION_POSITION_LOOP:
			steering_communication_SET_PID_PARAMETER_term_handler(&steering->motion_part.motor.PID_Handles.position_loop_handle, rx_data1_byte, term_offset_id);
			break;
		case OFFSET_MOTION_VELOCITY_LOOP:
			steering_communication_SET_PID_PARAMETER_term_handler(&steering->motion_part.motor.PID_Handles.velocity_loop_handle, rx_data1_byte, term_offset_id);
			break;
		default:
			break;
	}
	return rx_pack;
}

void steering_communication_queue_init(steering_communication_queue_t *queue)
{
	memset(queue, 0, sizeof(steering_communication_queue_t));
	queue->boundary = STEERING_COMMUNICATION_QUEUE_LENGTH;
}
/*
	用于将某个待发送队列中的pack发送了，还没写好
*/
void steering_communication_tx_queue_pop(steering_communication_ctx_t *ctx)
{

}
/*
	用于将某个pack放入待发送队列中，还没写好
*/
void steering_communication_queue_push(steering_communication_queue_t *queue, steering_communication_pack_t pack)
{
	//if (queue->tail != queue->)
}

static uint8_t platform_trans(void *handle, uint32_t Ext_CANID, uint8_t aData[])
{
    #if defined(STM32F105) | (STM32F407)
        steering_communication_CAN_TxHeaderStruct.ExtId = Ext_CANID;
        return HAL_CAN_AddTxMessage(handle, &steering_communication_CAN_TxHeaderStruct, aData, &steering_communication_pTxMailbox);
    #endif
}