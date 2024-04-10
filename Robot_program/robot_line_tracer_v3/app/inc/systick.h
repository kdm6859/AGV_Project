/*
 * (C) COPYRIGHT 2015 NCSLAB
 *
 * File Name : systick.h
 * Author    : SRLEE
 * Version   : V1.0
 * Date      : 28/12/2015
 */

#ifndef _SYSTICK_H_
#define _SYSTICK_H_

#include "stm32f10x.h"

void systick_init();
void delay_ms(__IO uint32_t nTime);
void TimingDelay_Decrement(void);

#endif  /*_SYSTICK_H_ */