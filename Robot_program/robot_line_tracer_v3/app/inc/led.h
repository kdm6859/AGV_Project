/*
 * (C) COPYRIGHT 2009 CRZ
 *
 * File Name : led.h
 * Author    : SRLEE
 * Version   : V1.0
 * Date      : 28/12/2015
 */

#ifndef _LED_H_
#define _LED_H_

#include "stm32f10x.h"

#define RCC_APB2Periph_GPIO_LED           RCC_APB2Periph_GPIOC

#define GPIO_LED                 GPIOC

#define GPIO_LED0_PIN          GPIO_Pin_6
#define GPIO_LED1_PIN          GPIO_Pin_7
#define GPIO_LED2_PIN          GPIO_Pin_8
#define GPIO_LED3_PIN          GPIO_Pin_9

void led_init();

void led0_on();
void led1_on();
void led2_on();
void led3_on();

void led0_off();
void led1_off();
void led2_off();
void led3_off();

void put_led(int val);


//my
void LED0_Toggle();

#endif /* _LED_H_ */