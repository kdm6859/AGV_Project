/*
 * (C) COPYRIGHT 2015 NCSLAB
 *
 * File Name : llio.h
 * Author    : SRLEE
 * Version   : V1.0
 * Date      : 24/12/2015
 */

#ifndef _LLIO_H_
#define _LLIO_H_
#include <stdio.h>
#include "stm32f10x.h"
#include <yfuns.h>

#define RCC_APB2Periph_GPIO_USART               RCC_APB2Periph_GPIOA
#define GPIO_USART                              GPIOA
#define GPIO_USART_Rx_Pin                       GPIO_Pin_10
#define GPIO_USART_Tx_Pin                       GPIO_Pin_9

//my
//#define RCC_APB2Periph_GPIO_USART3              RCC_APB2Periph_GPIOB
//#define GPIO_USART3                             GPIOB
//#define GPIO_USART3_Rx_Pin                      GPIO_Pin_11
//#define GPIO_USART3_Tx_Pin                      GPIO_Pin_10

//#define RCC_APB2Periph_GPIO_USART2              RCC_APB2Periph_GPIOA
//#define GPIO_USART2                             GPIOA
//#define GPIO_USART2_Rx_Pin                      GPIO_Pin_3
//#define GPIO_USART2_Tx_Pin                      GPIO_Pin_2

void llio_init();
int MyLowLevelPutchar(int ch);
int MyLowLevelGetchar();
size_t __write(int handle, const unsigned char * buffer, size_t size);
size_t __read(int handle, unsigned char * buffer, size_t size);
//void delay_ms(volatile unsigned int nTime);

//my
//void USART1_IT_init();
//void NVIC_USART1_Configuration();
//void Uart1_Push(uint16_t data);
//void USART3_init(int baudrate);
//void NVIC_USART3_Configuration();
//int MyLowLevelPutchar_Uart3(uint8_t ch);
//void USART2_init();
//int MyLowLevelPutchar_Uart2(int ch);
//int MyLowLevelGetchar_Uart2();

#endif  /*_LLIO_H_ */

