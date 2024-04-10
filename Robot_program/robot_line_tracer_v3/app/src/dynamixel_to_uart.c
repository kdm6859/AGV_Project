#include "dynamixel_to_uart.h"

/* initialize low level input/output using UART3 */
void USART3_init(int baudrate)
{
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /* Enable GPIOB clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_USART3, ENABLE);

    /* Enable USART3 clocks */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    /* Configure the GPIO ports */
    
    /* Configure USART3_Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin   = GPIO_USART3_Tx_Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIO_USART3, &GPIO_InitStructure);

    /* Configure USART3_Rx as input floating */
    GPIO_InitStructure.GPIO_Pin  = GPIO_USART3_Rx_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIO_USART3, &GPIO_InitStructure);

    /* USART3 configured as follow:
     - BaudRate = 115200 baud  
     - Word Length = 8 Bits
     - One Stop Bit
     - No parity
     - Hardware flow control disabled (RTS and CTS signals)
     - Receive and transmit enabled
    */
    USART_InitStructure.USART_BaudRate   = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits   = USART_StopBits_1;
    USART_InitStructure.USART_Parity     = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl
                                         = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;

    /* Configure the USART3 */ 
    USART_Init(USART3, &USART_InitStructure);
    
    /* USART3 Interrupt Enable */
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); 

    /* Enable the USART3 */
    USART_Cmd(USART3, ENABLE);
}

void NVIC_USART3_Configuration(){
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /*Set the Vector Table base location at 0x08000000*/
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
  
  /*Configure one bit for preemption priority*/
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  
  /*Enable the TIMx Interrupt*/
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

int MyLowLevelPutchar_Uart3(uint8_t ch)
{ 
    USART_SendData(USART3, (u8) ch);

    /* Loop until the end of transmission */
    while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);

    return ch;  
}