/*
 * (C) COPYRIGHT 2015 NCSLAB
 *
 * File Name : exti.h
 * Author    : SRLEE
 * Version   : V1.0
 * Date      : 24/12/2015
 */

#include "exti.h"

void nvic_init(void)
{ 
    NVIC_InitTypeDef NVIC_InitStructure;

#ifdef  VECT_TAB_RAM
    /* Set the Vector Table base location at 0x20000000 */ 
   // NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else  /* VECT_TAB_FLASH  */
    /* Set the Vector Table base location at 0x08000000 */ 
   // NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
#endif

    /* Configure one bit for preemption priority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    /* Enable the EXTI0 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Enable the EXTI1 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    /* Enable the EXTI2 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Enable the EXTI3 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void exti_init(void)
{ 
    EXTI_InitTypeDef EXTI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    /* NVIC init */
    nvic_init();

    /* clock init for AFIO */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    
    /* pins for external interrupt must be configured as input */
    GPIO_InitStructure.GPIO_Pin  = EXTI_KEY0_PIN | EXTI_KEY1_PIN | EXTI_KEY2_PIN | EXTI_KEY3_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(EXTI_KEY, &GPIO_InitStructure);
    
    /* Configure gpio as input : Button Key 0 */
    /* Connect EXTI Line to gpio pin */
    GPIO_EXTILineConfig(GPIO_PORTSOURCE_KEY, GPIO_PINSOURCE_KEY0);
        
    /* Configure EXTI Line to generate an interrupt */
    EXTI_InitStructure.EXTI_Line    = GPIO_EXTI_Line_KEY0;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Configure gpio as input : Button Key1 */
    /* Connect EXTI Line to gpio pin */
    GPIO_EXTILineConfig(GPIO_PORTSOURCE_KEY, GPIO_PINSOURCE_KEY1);

    /* Configure EXTI Line to generate an interrupt */  
    EXTI_InitStructure.EXTI_Line    = GPIO_EXTI_Line_KEY1;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
    /* Configure gpio as input : Button Key2 */
    /* Connect EXTI Line to gpio pin */
    GPIO_EXTILineConfig(GPIO_PORTSOURCE_KEY, GPIO_PINSOURCE_KEY2);

    /* Configure EXTI Line to generate an interrupt */  
    EXTI_InitStructure.EXTI_Line    = GPIO_EXTI_Line_KEY2;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
    /* Configure gpio as input : Button Key3 */
    /* Connect EXTI Line to gpio pin */
    GPIO_EXTILineConfig(GPIO_PORTSOURCE_KEY, GPIO_PINSOURCE_KEY3);

    /* Configure EXTI Line to generate an interrupt */  
    EXTI_InitStructure.EXTI_Line    = GPIO_EXTI_Line_KEY3;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
}
