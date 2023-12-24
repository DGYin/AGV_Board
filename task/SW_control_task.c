
#include "SW_control_task.h"

steering_wheel_t steering_wheel;

void SW_control_task(void)
{
    Steering_Wheel_CommandUpdate(&steering_wheel);
    Steering_Wheel_StatusUpdate(&steering_wheel);
	Steering_Wheel_CommandTransmit(&steering_wheel);

}

void SW_subscribe_task(void)
{

}

void SW_control_task_init(void)
{

    Steering_Wheel_HandleInit(&steering_wheel);
	steering_communication_init();
}
