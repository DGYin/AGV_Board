#include "M3508_gear_bsp.h"

void M3508_gear_feedback_process(M3508_gear_t *kit)
{
	kit->feedback.last_rotor_position_lsb = kit->feedback.current_rotor_position_lsb;
	kit->feedback.current_rotor_position_lsb = kit->parameter.bus->motor[kit->parameter.ESC_ID].feedback.LSB_rotor_position;
	kit->feedback.current_rotor_rpm = kit->parameter.bus->motor[kit->parameter.ESC_ID].feedback.LSB_rotor_rpm;
	
	// 单圈角度与多圈角度转换
	if (kit->feedback.current_rotor_position_lsb - kit->feedback.last_rotor_position_lsb < -8191/2) 
		kit->status.rotor_total_round++;
	if (kit->feedback.current_rotor_position_lsb - kit->feedback.last_rotor_position_lsb >  8191/2)
		kit->status.rotor_total_round--;
	kit->status.rotor_total_lsb = kit->status.rotor_total_round * 8192 + kit->feedback.current_rotor_position_lsb;

}

void M3508_gear_command_transmit(M3508_gear_t *kit, M3508_SINGLE_COMMAND_HOLD_t hold)
{
	M3508_set_single_motor_current(kit->parameter.bus, kit->parameter.ESC_ID, kit->command.torque_current_lsb, hold);
}