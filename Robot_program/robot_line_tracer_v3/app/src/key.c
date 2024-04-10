/*
 * (C) COPYRIGHT 2015 NCSLAB
 *
 * File Name : key.c
 * Author    : SRLEE
 * Version   : V1.0
 * Date      : 24/12/2015
 */

#include "key.h"
#include "stm32f10x.h"

void key_init()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /* Enable GPIOC clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_KEY, ENABLE);

    /* Configure KEY port */
    GPIO_InitStructure.GPIO_Pin  = GPIO_KEY0_PIN | GPIO_KEY1_PIN | GPIO_KEY2_PIN | GPIO_KEY3_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIO_KEY, &GPIO_InitStructure);
}

int get_key0()
{
    if(GPIO_ReadInputDataBit(GPIO_KEY, GPIO_KEY0_PIN) == Bit_SET) 
        return 1;
    else 
        return 0;  
}
int get_key1()
{
    if(GPIO_ReadInputDataBit(GPIO_KEY, GPIO_KEY1_PIN) == Bit_SET) 
        return 1;
    else 
        return 0;  
  
}
int get_key2()
{
    if(GPIO_ReadInputDataBit(GPIO_KEY, GPIO_KEY2_PIN) == Bit_SET) 
        return 1;
    else 
        return 0;  
  
}
int get_key3()
{
    if(GPIO_ReadInputDataBit(GPIO_KEY, GPIO_KEY3_PIN) == Bit_SET) 
        return 1;
    else 
        return 0;    
}

int get_key()
{
    int data=0;
    if(get_key0()) data |=  0x01;
    else data &=  ~0x01;
    if(get_key1()) data |=  0x02;
    else data &=  ~0x02;
    if(get_key2()) data |=  0x04;
    else data &=  ~0x04;
    if(get_key3()) data |=  0x08;
    else data &=  ~0x08;
    return data;
}
