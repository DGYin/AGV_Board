
// Comment TIM3_IRQHandler to enable this scheduler


// Include
#include "main.h"
#include "SW_control_task.h"

// Function Call



// Scheduler

void TASK_SCHEDULER(void)
{
	uint32_t tick = HAL_GetTick();
	SW_control_task();
}


