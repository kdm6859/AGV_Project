/*
 * (C) COPYRIGHT 2009 CRZ
 *
 * File Name : key.h
 * Author    : SRLEE
 * Version   : V1.0
 * Date      : 28/12/2015
 */

#ifndef _KEY_H_
#define _KEY_H_

#include "stm32f10x.h"

#define RCC_APB2Periph_GPIO_KEY           RCC_APB2Periph_GPIOC

#define GPIO_KEY                 GPIOC

#define GPIO_KEY0_PIN          GPIO_Pin_0
#define GPIO_KEY1_PIN          GPIO_Pin_1
#define GPIO_KEY2_PIN          GPIO_Pin_2
#define GPIO_KEY3_PIN          GPIO_Pin_3

void key_init();

int get_key0();
int get_key1();
int get_key2();
int get_key3();
int get_key();

#endif /* _KEY_H_ */