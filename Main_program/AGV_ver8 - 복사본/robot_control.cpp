#include "robot_control.h"

void robot_motor_control_value(int& r_speed_value, int& l_speed_value, int speed_value, int error_value, int gain) {
	r_speed_value = speed_value - gain * error_value;
	l_speed_value = speed_value + gain * error_value;

    if (r_speed_value > SPEED_VAL)
        r_speed_value = SPEED_VAL;
    else if (r_speed_value < -SPEED_VAL)
        r_speed_value = -SPEED_VAL;

    if (l_speed_value > SPEED_VAL)
        l_speed_value = SPEED_VAL;
    else if (l_speed_value < -SPEED_VAL)
        l_speed_value = -SPEED_VAL;

    /*l_speed_value = (l_speed_value < 0) ? 1024 - l_speed_value : l_speed_value;
    r_speed_value = (r_speed_value < 0) ? -r_speed_value : r_speed_value + 1024;*/
}