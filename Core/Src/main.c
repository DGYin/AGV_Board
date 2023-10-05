/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "can.h"
#include "tim.h"
#include "gpio.h"
#include "M3508.h"
#include "M3508_gear.h"
#include "briter_encoder.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_CAN1_Filter_Config(void)
{
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
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	CAN_RxHeaderTypeDef CAN_RxHeaderStruct;
	uint8_t rxdata[8];
	int16_t speed,*gdata,current;
	float angle;
	if(hcan==&hcan1)
	{
		HAL_CAN_GetRxMessage(&hcan1,CAN_RX_FIFO0,&CAN_RxHeaderStruct,rxdata);
		briter_encoder_feedback_handler(&briter_encoder, rxdata);
		M3508_feedback_handler(&M3508_bus_1, CAN_RxHeaderStruct.StdId, rxdata);
		M3508_gear_feedback_handler(&example_M3508_gear);
		
		switch(CAN_RxHeaderStruct.StdId)
		{
			
		}
	}

}




void canTX(void)
{
	uint8_t data[8];
	uint8_t status;
	CAN_TxHeaderTypeDef CAN_TxHeaderStruct;
	uint32_t  pTxMailbox;
	
    CAN_TxHeaderStruct.StdId=0x200;
    CAN_TxHeaderStruct.ExtId=0;
    CAN_TxHeaderStruct.DLC=8;
    CAN_TxHeaderStruct.IDE=CAN_ID_STD;
    CAN_TxHeaderStruct.RTR=CAN_RTR_DATA;
    CAN_TxHeaderStruct.TransmitGlobalTime=DISABLE;
	
	data[0] = 0xFF;
	data[1] = 0x01;
	data[2] = 0xFF;
	data[3] = 0x01;
	data[4] = 0xFF;
	data[5] = 0x01;
	data[6] = 0xFF;
	data[7] = 0x01;

	status = HAL_CAN_AddTxMessage(&hcan1,&CAN_TxHeaderStruct,data,&pTxMailbox);
	status = HAL_CAN_AddTxMessage(&hcan2,&CAN_TxHeaderStruct,data,&pTxMailbox);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CAN1_Init();
  MX_CAN2_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  HAL_CAN1_Filter_Config();
  HAL_CAN_Start(&hcan1);
  HAL_CAN_Start(&hcan2);
  HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING); 
  HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING); 
  
  M3508_gear_init();
  briter_encoder_Init();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  //M3508_set_single_motor_current(&M3508_bus_1, 4, 0xFF, SEND_COMMAND_NOW);
	  M3508_gear_set_torque_current_lsb(&example_M3508_gear, 0xFF, SEND_COMMAND_NOW);
	  briter_encoder_request_tatal_angle(&briter_encoder);
	  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
	  HAL_Delay(100);
	  
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.Prediv1Source = RCC_PREDIV1_SOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  RCC_OscInitStruct.PLL2.PLL2State = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the Systick interrupt time
  */
  __HAL_RCC_PLLI2S_ENABLE();
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
