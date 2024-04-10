/*
 * (C) COPYRIGHT 2015 NCSLAB
 *
 * File Name : exti.h
 * Author    : SRLEE
 * Version   : V1.0
 * Date      : 24/12/2015
 */

#ifndef _EXTI_H_
#define _EXTI_H_

#include "stm32f10x.h"

// keys are connected to PC0~PC3 pins
#define GPIO_PORTSOURCE_KEY        GPIO_PortSourceGPIOC
#define GPIO_PINSOURCE_KEY0        GPIO_PinSource0
#define GPIO_PINSOURCE_KEY1        GPIO_PinSource1
#define GPIO_PINSOURCE_KEY2        GPIO_PinSource2
#define GPIO_PINSOURCE_KEY3        GPIO_PinSource3

#define GPIO_EXTI_Line_KEY0        EXTI_Line0
#define GPIO_EXTI_Line_KEY1        EXTI_Line1
#define GPIO_EXTI_Line_KEY2        EXTI_Line2
#define GPIO_EXTI_Line_KEY3        EXTI_Line3

#define EXTI_KEY                 GPIOC
#define EXTI_KEY0_PIN          GPIO_Pin_0
#define EXTI_KEY1_PIN          GPIO_Pin_1
#define EXTI_KEY2_PIN          GPIO_Pin_2
#define EXTI_KEY3_PIN          GPIO_Pin_3

void nvic_init(void);
void exti_init(void);


#endif  /*_EXTI_H_ */