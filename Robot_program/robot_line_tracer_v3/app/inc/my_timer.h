#ifndef _MY_TIMER_H_
#define _MY_TIMER_H_

#include "stm32f10x_conf.h"
#include "led.h"

struct my_timespec {
        uint32_t    tv_sec;      /* seconds */
        uint32_t    tv_msec;     /* microseconds */
};

void RCC_Configuration(uint32_t RCC_APB1Periph_TIMx);
void NVIC_Configuration(IRQn_Type TIMx_IRQn, int PreemptionPriority_val, int SubPriority_val);
void TIM_Configuration(uint16_t TIM_Period_val, uint16_t TIM_Prescaler_val, TIM_TypeDef* TIMx);
void Timer_1_msec_Init();
void msecCounter(void);
int my_clock_gettime(struct my_timespec *tp);

#endif 