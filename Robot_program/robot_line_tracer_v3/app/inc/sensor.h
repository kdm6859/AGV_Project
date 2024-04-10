#ifndef _SENSOR_H_
#define _SENSOR_H_

#include "stm32f10x.h"

#define RCC_APB2Periph_GPIO_SENSOR           RCC_APB2Periph_GPIOA

#define GPIO_SENSOR                 GPIOA

#define GPIO_SENSOR0_PIN          GPIO_Pin_1
#define GPIO_SENSOR1_PIN          GPIO_Pin_4
#define GPIO_SENSOR2_PIN          GPIO_Pin_5
#define GPIO_SENSOR3_PIN          GPIO_Pin_6

void sensor_init();
int get_sensor();
int get_error(int value);

#endif 