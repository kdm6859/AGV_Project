/*
 * (C) COPYRIGHT 2015 NCSLAB
 *
 * File Name : llio.c
 * Author    : SRLEE
 * Version   : V1.0
 * Date      : 24/12/2015
 */

#include "llio.h"

/* initialize low level input/output using UART1 */
void llio_init()
{
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /* Enable GPIOA clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_USART, ENABLE);

    /* Enable USART1 clocks */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    /* Configure the GPIO ports */
    
    /* Configure USART1_Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin   = GPIO_USART_Tx_Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIO_USART, &GPIO_InitStructure);

    /* Configure USART1_Rx as input floating */
    GPIO_InitStructure.GPIO_Pin  = GPIO_USART_Rx_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIO_USART, &GPIO_InitStructure);

    /* USART1 configured as follow:
     - BaudRate = 115200 baud  
     - Word Length = 8 Bits
     - One Stop Bit
     - No parity
     - Hardware flow control disabled (RTS and CTS signals)
     - Receive and transmit enabled
    */
    USART_InitStructure.USART_BaudRate   = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits   = USART_StopBits_1;
    USART_InitStructure.USART_Parity     = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl
                                         = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;

    /* Configure the USART1 */ 
    USART_Init(USART1, &USART_InitStructure);

    /* Enable the USART1 */
    USART_Cmd(USART1, ENABLE);
}

//void USART1_IT_init()
//{
//    USART_InitTypeDef USART_InitStructure;
//    GPIO_InitTypeDef GPIO_InitStructure;
//    
//    /* Enable GPIOA clock */
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_USART, ENABLE);
//
//    /* Enable USART1 clocks */
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
//
//    /* Configure the GPIO ports */
//    
//    /* Configure USART1_Tx as alternate function push-pull */
//    GPIO_InitStructure.GPIO_Pin   = GPIO_USART_Tx_Pin;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
//    GPIO_Init(GPIO_USART, &GPIO_InitStructure);
//
//    /* Configure USART1_Rx as input floating */
//    GPIO_InitStructure.GPIO_Pin  = GPIO_USART_Rx_Pin;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//    GPIO_Init(GPIO_USART, &GPIO_InitStructure);
//
//    /* USART1 configured as follow:
//     - BaudRate = 115200 baud  
//     - Word Length = 8 Bits
//     - One Stop Bit
//     - No parity
//     - Hardware flow control disabled (RTS and CTS signals)
//     - Receive and transmit enabled
//    */
//    USART_InitStructure.USART_BaudRate   = 115200;
//    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
//    USART_InitStructure.USART_StopBits   = USART_StopBits_1;
//    USART_InitStructure.USART_Parity     = USART_Parity_No ;
//    USART_InitStructure.USART_HardwareFlowControl
//                                         = USART_HardwareFlowControl_None;
//    USART_InitStructure.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;
//
//    /* Configure the USART1 */ 
//    USART_Init(USART1, &USART_InitStructure);
//    
//    USART_ITConfig (USART1, USART_IT_RXNE, ENABLE); 
//
//    /* Enable the USART1 */
//    USART_Cmd(USART1, ENABLE);
//}

//void NVIC_USART1_Configuration(){
//  NVIC_InitTypeDef NVIC_InitStructure;
//  
//  /*Set the Vector Table base location at 0x08000000*/
//  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
//  
//  /*Configure one bit for preemption priority*/
//  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
//  
//  /*Enable the TIMx Interrupt*/
//  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
//}

//void Uart1_Push(uint16_t data){
//  printf("%c\n", data);
//}

///* initialize low level input/output using UART3 */
//void USART3_init(int baudrate)
//{
//    USART_InitTypeDef USART_InitStructure;
//    GPIO_InitTypeDef GPIO_InitStructure;
//    
//    /* Enable GPIOB clock */
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_USART3, ENABLE);
//
//    /* Enable USART3 clocks */
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
//
//    /* Configure the GPIO ports */
//    
//    /* Configure USART3_Tx as alternate function push-pull */
//    GPIO_InitStructure.GPIO_Pin   = GPIO_USART3_Tx_Pin;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
//    GPIO_Init(GPIO_USART3, &GPIO_InitStructure);
//
//    /* Configure USART3_Rx as input floating */
//    GPIO_InitStructure.GPIO_Pin  = GPIO_USART3_Rx_Pin;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//    GPIO_Init(GPIO_USART3, &GPIO_InitStructure);
//
//    /* USART3 configured as follow:
//     - BaudRate = 115200 baud  
//     - Word Length = 8 Bits
//     - One Stop Bit
//     - No parity
//     - Hardware flow control disabled (RTS and CTS signals)
//     - Receive and transmit enabled
//    */
//    USART_InitStructure.USART_BaudRate   = baudrate;
//    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
//    USART_InitStructure.USART_StopBits   = USART_StopBits_1;
//    USART_InitStructure.USART_Parity     = USART_Parity_No ;
//    USART_InitStructure.USART_HardwareFlowControl
//                                         = USART_HardwareFlowControl_None;
//    USART_InitStructure.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;
//
//    /* Configure the USART3 */ 
//    USART_Init(USART3, &USART_InitStructure);
//    
//    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); 
//
//    /* Enable the USART3 */
//    USART_Cmd(USART3, ENABLE);
//}

//void NVIC_USART3_Configuration(){
//  NVIC_InitTypeDef NVIC_InitStructure;
//  
//  /*Set the Vector Table base location at 0x08000000*/
//  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
//  
//  /*Configure one bit for preemption priority*/
//  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
//  
//  /*Enable the TIMx Interrupt*/
//  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
//}

/* initialize low level input/output using UART2 */
//void USART2_init()
//{
//    USART_InitTypeDef USART_InitStructure;
//    GPIO_InitTypeDef GPIO_InitStructure;
//    
//    /* Enable GPIOA clock */
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_USART2, ENABLE);
//
//    /* Enable USART2 clocks */
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
//
//    /* Configure the GPIO ports */
//    
//    /* Configure USART2_Tx as alternate function push-pull */
//    GPIO_InitStructure.GPIO_Pin   = GPIO_USART2_Tx_Pin;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
//    GPIO_Init(GPIO_USART2, &GPIO_InitStructure);
//
//    /* Configure USART2_Rx as input floating */
//    GPIO_InitStructure.GPIO_Pin  = GPIO_USART2_Rx_Pin;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//    GPIO_Init(GPIO_USART2, &GPIO_InitStructure);
//
//    /* USART2 configured as follow:
//     - BaudRate = 115200 baud  
//     - Word Length = 8 Bits
//     - One Stop Bit
//     - No parity
//     - Hardware flow control disabled (RTS and CTS signals)
//     - Receive and transmit enabled
//    */
//    USART_InitStructure.USART_BaudRate   = 115200;
//    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
//    USART_InitStructure.USART_StopBits   = USART_StopBits_1;
//    USART_InitStructure.USART_Parity     = USART_Parity_No ;
//    USART_InitStructure.USART_HardwareFlowControl
//                                         = USART_HardwareFlowControl_None;
//    USART_InitStructure.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;
//
//    /* Configure the USART2 */ 
//    USART_Init(USART2, &USART_InitStructure);
//
//    /* Enable the USART2 */
//    USART_Cmd(USART2, ENABLE);
//}

/* low level output impelementation, 
Refer to \Embedded Workbench 7.3\arm\src\lib\write.c and 
\Embedded Workbench 7.3\arm\doc\EWARM_DevelopmentGuide.ENU.pdf */
int MyLowLevelPutchar(int ch)
{
    /* Write a character to the USART */  
    if( ch == '\n') {
        USART_SendData(USART1, '\r');
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
        USART_SendData(USART1, '\n');
    }else {
        USART_SendData(USART1, (u8) ch);
    }

    /* Loop until the end of transmission */
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);

    return ch;  
}

//int MyLowLevelPutchar_Uart3(uint8_t ch)
//{ 
//    USART_SendData(USART3, (u8) ch);
//
//    /* Loop until the end of transmission */
//    while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
//
//    return ch;  
//}

//int MyLowLevelPutchar_Uart2(int ch)
//{
//    /* Write a character to the USART */  
//    if( ch == '\n') {
//        USART_SendData(USART2, '\r');
//        while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
//        USART_SendData(USART2, '\n');
//    }else {
//        USART_SendData(USART2, (u8) ch);
//    }
//
//    /* Loop until the end of transmission */
//    while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
//
//    return ch;  
//}

size_t __write(int handle, const unsigned char * buffer, size_t size)
{
  /* Remove the #if #endif pair to enable the implementation */
    size_t nChars = 0;

    if (buffer == 0)
    {
    /*
     * This means that we should flush internal buffers.  Since we
     * don't we just return.  (Remember, "handle" == -1 means that all
     * handles should be flushed.)
     */
        return 0;
    }

    /* This template only writes to "standard out" and "standard err",
    * for all other file handles it returns failure. */
    if (handle != _LLIO_STDOUT && handle != _LLIO_STDERR)
    {
        return _LLIO_ERROR;
    }

    for (/* Empty */; size != 0; --size)
    {
        if (MyLowLevelPutchar(*buffer++) < 0)
        {
            return _LLIO_ERROR;
        }

        ++nChars;
    }

    return nChars;
}

/* low level input impelementation, 
Refer to \Embedded Workbench 7.3\arm\src\lib\read.c and 
\Embedded Workbench 7.3\arm\doc\EWARM_DevelopmentGuide.ENU.pdf */
int MyLowLevelGetchar()
{  
    int data;
    /* loop until the end of transmission */
    while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
    /* Write a character to the USART */
    data = USART_ReceiveData(USART1);
    return data; 
}

//int MyLowLevelGetchar_Uart2()
//{  
//    int data;
//    /* loop until the end of transmission */
//    while(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);
//    /* Write a character to the USART */
//    data = USART_ReceiveData(USART2);
//    return data; 
//}

size_t __read(int handle, unsigned char * buffer, size_t size)
{
    /* Remove the #if #endif pair to enable the implementation */
    int nChars = 0;

    /* This template only reads from "standard in", for all other file
    * handles it returns failure. */
    if (handle != _LLIO_STDIN)
    {
        return _LLIO_ERROR;
    }

    for (/* Empty */; size > 0; --size)
    {
        int c = MyLowLevelGetchar();
        if (c < 0)
            break;

        *buffer++ = c;
        ++nChars;
    }

    return nChars;
}

//int 

/*
// delay function using for loop
void delay_ms(volatile unsigned int nTime)
{
  // 1sec = 806596 in case of SYSCLK=HSI=8Mhz
    nTime *= 807*9;
    for(;nTime>0;nTime--);
  
}
*/