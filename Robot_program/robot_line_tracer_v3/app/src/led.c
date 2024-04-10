/*
 * (C) COPYRIGHT 2015 NCSLAB
 *
 * File Name : led.c
 * Author    : SRLEE
 * Version   : V1.0
 * Date      : 24/12/2015
 */

#include "led.h"
#include "stm32f10x.h"

void led_init()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /* Enable GPIOC clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_LED, ENABLE);

    /* Configure LED port */
    GPIO_InitStructure.GPIO_Pin   = GPIO_LED0_PIN | GPIO_LED1_PIN | GPIO_LED2_PIN | GPIO_LED3_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIO_LED, &GPIO_InitStructure);
    
}

void led0_on()
{
    GPIO_SetBits(GPIO_LED, GPIO_LED0_PIN);
}
void led0_off()
{
    GPIO_ResetBits(GPIO_LED, GPIO_LED0_PIN);
}
void led1_on()
{
    GPIO_SetBits(GPIO_LED, GPIO_LED1_PIN);
}
void led1_off()
{
    GPIO_ResetBits(GPIO_LED, GPIO_LED1_PIN);
}
void led2_on()
{
    GPIO_SetBits(GPIO_LED, GPIO_LED2_PIN);
}
void led2_off()
{
    GPIO_ResetBits(GPIO_LED, GPIO_LED2_PIN);
}
void led3_on()
{
    GPIO_SetBits(GPIO_LED, GPIO_LED3_PIN);
}
void led3_off()
{
    GPIO_ResetBits(GPIO_LED, GPIO_LED3_PIN);
}

void put_led(int val)
{
    if(val & 0x01) led0_on();
    else led0_off();
    if(val & 0x02) led1_on();
    else led1_off();
    if(val & 0x04) led2_on();
    else led2_off();
    if(val & 0x08) led3_on();
    else led3_off();
    
}





//my
void LED0_Toggle(){
  static uint8_t tog = 0;
  if(tog == 0){
    led0_off();
    tog = 1;
  }
  else{ 
    led0_on();
    tog = 0;
  }
}