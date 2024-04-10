#ifndef __ROBOT_CONTROL_H__
#define __ROBOT_CONTROL_H__

#include <iostream>

#define SPEED_VAL	1023

void robot_motor_control_value(int& r_speed_value, int& l_speed_value, int speed_value, int error_value, int gain);

#endif