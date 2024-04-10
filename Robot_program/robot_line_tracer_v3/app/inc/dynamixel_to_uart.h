#ifndef _DYNAMIXEL_TO_UART_
#define _DYNAMIXEL_TO_UART_

#include <stdio.h>
#include "stm32f10x.h"

#define RCC_APB2Periph_GPIO_USART3              RCC_APB2Periph_GPIOB
#define GPIO_USART3                             GPIOB
#define GPIO_USART3_Rx_Pin                      GPIO_Pin_11
#define GPIO_USART3_Tx_Pin                      GPIO_Pin_10

void USART3_init(int baudrate);
void NVIC_USART3_Configuration();
int MyLowLevelPutchar_Uart3(uint8_t ch);

#endif  /*_DYNAMIXEL_TO_UART_ */

