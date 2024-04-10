#ifndef _WIFI_TO_UART_H_
#define _WIFI_TO_UART_H_

#include <stdio.h>
#include "stm32f10x.h"
#include "systick.h"

#define RCC_APB2Periph_GPIO_WifiReset           RCC_APB2Periph_GPIOC
#define RCC_APB2Periph_GPIO_USART2              RCC_APB2Periph_GPIOA
#define GPIO_WifiReset                          GPIOC
#define GPIO_USART2                             GPIOA
#define GPIO_WifiReset_PIN                      GPIO_Pin_0
#define GPIO_USART2_Rx_Pin                      GPIO_Pin_3
#define GPIO_USART2_Tx_Pin                      GPIO_Pin_2

//my
void WIFI_reset_init();
void USART2_init();
void NVIC_USART2_Configuration();
int MyLowLevelPutchar_Uart2(int ch);
//int MyLowLevelGetchar_Uart2();

#endif  /*_WIFI_TO_UART_H_ */