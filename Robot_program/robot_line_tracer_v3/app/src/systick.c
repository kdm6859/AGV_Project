/*
 * (C) COPYRIGHT 2015 NCSLAB
 *
 * File Name : systick.c
 * Author    : SRLEE
 * Version   : V1.0
 * Date      : 28/12/2015
 */

#include "systick.h"
#include <stdio.h>

static volatile uint32_t TimingDelay;

// this function must be called after clock configuration.
void systick_init()
{
    RCC_ClocksTypeDef  rcc_clocks;  
    RCC_GetClocksFreq(&rcc_clocks);

    /* Setup SysTick Timer for 1 msec interrupts  */
    if(SysTick_Config(rcc_clocks.SYSCLK_Frequency / 1000))
    {
        printf("SysTick_Config error\n");  
        while(1);
    }
    //printf("SYSCLK_Freq->%d\n",rcc_clocks.SYSCLK_Frequency);
}

// delay function using systick interrupt
void delay_ms(__IO uint32_t nTime)
{ 
    TimingDelay = nTime;
    while(TimingDelay != 0);
}

// refer to SysTick_Handler() in stm32f10x_it.c
void TimingDelay_Decrement(void)
{
    if (TimingDelay != 0x00)
    { 
        TimingDelay--;
    }
}