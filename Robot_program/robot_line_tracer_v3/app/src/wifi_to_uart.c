#include "wifi_to_uart.h"

void WIFI_reset_init(){
  systick_init();
  
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_WifiReset, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin   = GPIO_WifiReset_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_Init(GPIO_WifiReset, &GPIO_InitStructure);
  
  
  GPIO_ResetBits(GPIO_WifiReset, GPIO_WifiReset_PIN);
  delay_ms(10);
  GPIO_SetBits(GPIO_WifiReset, GPIO_WifiReset_PIN);
}

/* initialize low level input/output using UART2 */
void USART2_init()
{
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /* Enable GPIOA clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_USART2, ENABLE);

    /* Enable USART2 clocks */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    /* Configure the GPIO ports */
    
    /* Configure USART2_Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin   = GPIO_USART2_Tx_Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIO_USART2, &GPIO_InitStructure);

    /* Configure USART2_Rx as input floating */
    GPIO_InitStructure.GPIO_Pin  = GPIO_USART2_Rx_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIO_USART2, &GPIO_InitStructure);

    /* USART2 configured as follow:
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

    /* Configure the USART2 */ 
    USART_Init(USART2, &USART_InitStructure);
    
    /* USART2 Interrupt Enable */
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); 

    /* Enable the USART2 */
    USART_Cmd(USART2, ENABLE);
}

void NVIC_USART2_Configuration(){
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /*Set the Vector Table base location at 0x08000000*/
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
  
  /*Configure one bit for preemption priority*/
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  
  /*Enable the TIMx Interrupt*/
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

int MyLowLevelPutchar_Uart2(int ch)
{
    /* Write a character to the USART */  
    if( ch == '\n') {
        USART_SendData(USART2, '\r');
        while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
        USART_SendData(USART2, '\n');
    }else {
        USART_SendData(USART2, (u8) ch);
    }

    /* Loop until the end of transmission */
    while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);

    return ch;  
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