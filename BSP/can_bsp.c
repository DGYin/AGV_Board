#include "can_bsp.h"
#include "briter_encoder.h"
#include "M3508_gear.h"
#include "SW_control_task.h"
#if defined(STM32F105) | defined(STM32F407)
	#include "can.h"
#endif

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	CAN_RxHeaderTypeDef CAN_RxHeaderStruct;
	uint8_t rxdata[8];
	int16_t speed,*gdata,current;
	float angle;
	if(hcan==&hcan1)
	{
		HAL_CAN_GetRxMessage(&hcan1,CAN_RX_FIFO0,&CAN_RxHeaderStruct,rxdata);

		//briter_encoder_feedback_handler(&briter_encoder, rxdata);
		M3508_feedback_handler(&M3508_bus_1, CAN_RxHeaderStruct.StdId, rxdata);
		M3508_gear_feedback_handler(&steering_wheel.directive_part.motor.M3508_kit);
		M3508_gear_feedback_handler(&steering_wheel.motion_part.motor.M3508_kit);
		
		
		// 因为switch只能用常量，所以改用If
		if (CAN_RxHeaderStruct.StdId == steering_wheel.directive_part.encoder.briter_encoder.parameter.CAN_ID)
		{
			briter_encoder_feedback_handler(&steering_wheel.directive_part.encoder.briter_encoder, rxdata);
		}
		


	}

}

void platform_filtter_config_setting(void)
{
	#if defined(STM32F105) | defined(STM32F407)
		CAN_FilterTypeDef CAN_FilterStructure;
		CAN_FilterStructure.FilterActivation = ENABLE;
		CAN_FilterStructure.FilterBank = 1;
		CAN_FilterStructure.FilterMode = CAN_FILTERMODE_IDMASK;
		CAN_FilterStructure.FilterScale = CAN_FILTERSCALE_32BIT;
		CAN_FilterStructure.FilterFIFOAssignment = CAN_FILTER_FIFO0;
		CAN_FilterStructure.FilterIdHigh = 0x0000;
		CAN_FilterStructure.FilterIdLow = 0x0000;
		CAN_FilterStructure.FilterMaskIdHigh = 0x0000;
		CAN_FilterStructure.FilterMaskIdLow = 0x0000;
		
		HAL_CAN_ConfigFilter(&hcan1,&CAN_FilterStructure);
		HAL_CAN_ConfigFilter(&hcan2,&CAN_FilterStructure);
	#endif
}

