#include "steering_wheel.h"

#if defined(STM32F105) | defined(STM32F407)
	#include "can.h"
#endif

steering_wheel_t steering_wheel;

void Steering_Wheel_PID_HandleInit(steering_wheel_t *steering_wheel)
{
	// 先把PID结构体置零
	memcpy(&steering_wheel->directive_part.direction_motor.PID_Handles	, 0, sizeof(steering_wheel->directive_part.direction_motor.PID_Handles));
	memcpy(&steering_wheel->motion_part.motion_motor.PID_Handles		, 0, sizeof(steering_wheel->motion_part.motion_motor.PID_Handles));
	// 转向电机角度环 默认Kp Ki Kd 写入
	steering_wheel->directive_part.direction_motor.PID_Handles.position_loop_handle.hDefKpGain = 0.08f;
	steering_wheel->directive_part.direction_motor.PID_Handles.position_loop_handle.hDefKiGain = 0.01f;
	// 转向电机速度环 默认Kp Ki Kd 写入
	steering_wheel->directive_part.direction_motor.PID_Handles.velocity_loop_handle.hDefKpGain = 4;
	steering_wheel->directive_part.direction_motor.PID_Handles.velocity_loop_handle.hDefKiGain = 0.6f;
	// 动力电机速度环 默认Kp Ki Kd 写入
	steering_wheel->motion_part.motion_motor.PID_Handles.velocity_loop_handle.hDefKpGain = 0.76;
	steering_wheel->motion_part.motion_motor.PID_Handles.velocity_loop_handle.hDefKiGain = 0.00265;
	steering_wheel->motion_part.motion_motor.PID_Handles.velocity_loop_handle.hDefKdGain = 0.1;
	// 调用PID结构体初始化函数，数据写入PID结构体中
	PID_HandleInit(&steering_wheel->directive_part.direction_motor.PID_Handles.position_loop_handle);
	PID_HandleInit(&steering_wheel->directive_part.direction_motor.PID_Handles.velocity_loop_handle);
	PID_HandleInit(&steering_wheel->motion_part.motion_motor.PID_Handles.velocity_loop_handle);
}

void Steering_Wheel_HandleInit(steering_wheel_t *steering_wheel)
{
	// 初始化PID结构体
	Steering_Wheel_PID_HandleInit(steering_wheel);
	#if defined(DIRECTIVE_MOTOR_M3508) | defined(MOTION_MOTOR_M3508)
		// 转向电机及其齿轮初始化
		M3508_gear_parameter_t direction_motor_gear_init;
		direction_motor_gear_init.reduction_rate	= 10;
		direction_motor_gear_init.bus				= &M3508_bus_1;
		direction_motor_gear_init.ESC_ID			= 1;
		direction_motor_gear_init.resistance_torque	= 0;
		M3508_gear_parameter_init(&steering_wheel->directive_part.direction_motor.M3508_motor, &direction_motor_gear_init);
		// 动力电机及其齿轮初始化
		M3508_gear_parameter_t motion_motor_gear_init;
		motion_motor_gear_init.reduction_rate		= 3591 / 187.f;
		motion_motor_gear_init.bus					= &M3508_bus_1;
		motion_motor_gear_init.ESC_ID				= 5;
		motion_motor_gear_init.resistance_torque	= 0;
		M3508_gear_parameter_init(&steering_wheel->motion_part.motion_motor.M3508_motor, &motion_motor_gear_init);
	#endif
	
	#if defined(DIRECTIVE_ENCODER_BRITER_ENCODER)
		// 编码器初始化
		birter_encoder_parameter_t birter_encoder_init;
		birter_encoder_init.baud_rate				= BRITER_ENCODER_SET_CAN_BAUD_RATE_1M;
		birter_encoder_init.call_back_mode			= BRITER_ENCODER_SET_CALLBACK_REQUEST;
		birter_encoder_init.increment_direction		= BRITER_ENCODER_INCREMENT_DIRECTION_CW;
		birter_encoder_init.CAN_ID					= 0x0A;
		briter_encoder_parameter_init(&steering_wheel->directive_part.encoder.briter_encoder, &birter_encoder_init);
	#endif
}


STEERING_WHEEL_RETURN_T Steering_Wheel_RawDataUpdate(steering_wheel_t *steering_wheel)
{
	// 获取编码器多圈角度
    #if defined(DIRECTIVE_ENCODER_BRITER_ENCODER)
		steering_wheel->directive_part.encoder.feedback.position = steering_wheel->directive_part.encoder.briter_encoder.status.total_angle;
		briter_encoder_request_tatal_angle(&steering_wheel->directive_part.encoder.briter_encoder);
	#endif
	// 获取转向电机经过齿轮传动后的角度、速度、力矩
	#if defined(DIRECTIVE_MOTOR_M3508)
		steering_wheel->directive_part.direction_motor.feedback.position	= steering_wheel->directive_part.direction_motor.M3508_motor.status.output_angle_lsb;
		steering_wheel->directive_part.direction_motor.feedback.speed		= steering_wheel->directive_part.direction_motor.M3508_motor.status.output_speed_rpm;
		steering_wheel->directive_part.direction_motor.feedback.torque		= steering_wheel->directive_part.direction_motor.M3508_motor.status.output_torque;
	#endif
	// 获取动力电机经过齿轮传动后的角度、速度、力矩
	#if defined(DIRECTIVE_MOTOR_M3508)
		steering_wheel->motion_part.motion_motor.feedback.position	= steering_wheel->motion_part.motion_motor.M3508_motor.status.output_angle_lsb;
		steering_wheel->motion_part.motion_motor.feedback.speed		= steering_wheel->motion_part.motion_motor.M3508_motor.status.output_speed_rpm;
		steering_wheel->motion_part.motion_motor.feedback.torque	= steering_wheel->motion_part.motion_motor.M3508_motor.status.output_torque;
	#endif
	return STEERING_WHEEL_OK;
}

STEERING_WHEEL_RETURN_T Steering_Wheel_PID_regulator(steering_wheel_t *steering_wheel)
{
	// 动力电机角度环PID
	PID_Controller(&steering_wheel->motion_part.motion_motor.PID_Handles.velocity_loop_handle, );
	return STEERING_WHEEL_OK;
}