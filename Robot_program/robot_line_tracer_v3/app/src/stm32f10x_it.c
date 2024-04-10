/**
  ******************************************************************************
  * @file    SysTick/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.1.0
  * @date    06/19/2009
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @copy
  *
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIMFE. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_dma.h"
#include "systick.h"
#include "exti.h"
#include <stdio.h>
#include "debug.h"    

#include "led.h"
#include "llio.h"
#include "circle_queue.h"
#include "my_timer.h"
    
extern Queue buffer, uart2_buffer;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1)
    {
    }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1)
    {
    }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1)
    {
    }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
    TimingDelay_Decrement();
}

/******************************************************************************/
/*            STM32F10x Peripherals Interrupt Handlers                        */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/*******************************************************************************
* Function Name  : WWDG_IRQHandler
* Description    : This function handles WWDG interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WWDG_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : PVD_IRQHandler
* Description    : This function handles PVD interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PVD_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TAMPER_IRQHandler
* Description    : This function handles Tamper interrupt request. 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TAMPER_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : RTC_IRQHandler
* Description    : This function handles RTC global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : FLASH_IRQHandler
* Description    : This function handles Flash interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FLASH_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : RCC_IRQHandler
* Description    : This function handles RCC interrupt request. 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : EXTI0_IRQHandler
* Description    : This function handles External interrupt Line 0 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void EXTI0_IRQHandler(void)
{   
    if(EXTI_GetITStatus(GPIO_EXTI_Line_KEY1) != RESET) {
        printf("Key0 Button Pressed\n");
        EXTI_ClearITPendingBit(GPIO_EXTI_Line_KEY1);
    }
}

/*
static uint8_t Send_Buffer[5];
static bool toggle_data_key0 = FALSE;

void EXTI0_IRQHandler(void)
{
    DEBUG_MSG_FUNC_START;  
  if(EXTI_GetITStatus(GPIO_EXTI_Line_KEY0) != RESET) {
       
#if 0
        // USB Remote Wakup ... 
        if (pInformation->Current_Feature & 0x20) //Remote wake-up enabled
        {
          Resume(RESUME_INTERNAL);
        }
#endif

        Send_Buffer[0] = 0x05;

        if(toggle_data_key0)
        {
            toggle_data_key0 = FALSE;
            Send_Buffer[1] = 0x01;
            Send_Buffer[2] = 0x01;
            Send_Buffer[3] = 0x01;
            Send_Buffer[4] = 0x01;
            printf("Key0 Button : TRUE\n");
        }
        else
        {
            toggle_data_key0 = TRUE;
            Send_Buffer[1] = 0x00;
            Send_Buffer[2] = 0x00;
            Send_Buffer[3] = 0x00;
            Send_Buffer[4] = 0x00;
            printf("Key0 Button : FALSE\n");
        }

        UserToPMABufferCopy(Send_Buffer, ENDP1_TXADDR, 5);
        SetEPTxCount(ENDP1, 5);
        //UserToPMABufferCopy(Send_Buffer, ENDP1_TXADDR, 2);
        //SetEPTxCount(ENDP1, 2);
        SetEPTxValid(ENDP1);

        EXTI_ClearITPendingBit(GPIO_EXTI_Line_KEY0);
    }
}
*/
/*******************************************************************************
* Function Name  : EXTI1_IRQHandler
* Description    : This function handles External interrupt Line 1 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void EXTI1_IRQHandler(void)
{
    if(EXTI_GetITStatus(GPIO_EXTI_Line_KEY1) != RESET) {
        printf("Key1 Button Pressed\n");
        EXTI_ClearITPendingBit(GPIO_EXTI_Line_KEY1);
    }
}

/*
static bool toggle_data_key1 = FALSE;

void EXTI1_IRQHandler(void)
{
    DEBUG_MSG_FUNC_START;
    if(EXTI_GetITStatus(GPIO_EXTI_Line_KEY1) != RESET) {
       
        Send_Buffer[0] = 0x06;

        if(toggle_data_key1)
        {
            toggle_data_key1 = FALSE;
            Send_Buffer[1] = 0x01;
            printf("Key1 Button : TRUE\n");
        }
        else
        {
            toggle_data_key1 = TRUE;
            Send_Buffer[1] = 0x00;
            printf("Key1 Button : FALSE\n");
        }

        UserToPMABufferCopy(Send_Buffer, ENDP1_TXADDR, 2);
        SetEPTxCount(ENDP1, 2);
        SetEPTxValid(ENDP1);

        EXTI_ClearITPendingBit(GPIO_EXTI_Line_KEY1);
    }
}
*/
/*******************************************************************************
* Function Name  : EXTI2_IRQHandler
* Description    : This function handles External interrupt Line 2 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void EXTI2_IRQHandler(void)
{
     if(EXTI_GetITStatus(GPIO_EXTI_Line_KEY2) != RESET) {
        printf("Key2 Button Pressed\n");
        EXTI_ClearITPendingBit(GPIO_EXTI_Line_KEY2);
    }
}
/*
static bool toggle_data_key2 = FALSE;
void EXTI2_IRQHandler(void)
{
    DEBUG_MSG_FUNC_START;  
    if(EXTI_GetITStatus(GPIO_EXTI_Line_KEY2) != RESET) {
       
        Send_Buffer[0] = 0x07;

        if(toggle_data_key2)
        {
            toggle_data_key2 = FALSE;
            Send_Buffer[1] = 0x01;
            printf("Key2 Button : TRUE\n");
        }
        else
        {
            toggle_data_key2 = TRUE;
            Send_Buffer[1] = 0x00;
            printf("Key2 Button : FALSE\n");
        }

        UserToPMABufferCopy(Send_Buffer, ENDP1_TXADDR, 2);
        SetEPTxCount(ENDP1, 2);
        SetEPTxValid(ENDP1);

        EXTI_ClearITPendingBit(GPIO_EXTI_Line_KEY2);
    }
}
*/
/*******************************************************************************
* Function Name  : EXTI3_IRQHandler
* Description    : This function handles External interrupt Line 3 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void EXTI3_IRQHandler(void)
{
     if(EXTI_GetITStatus(GPIO_EXTI_Line_KEY3) != RESET) {
        printf("Key3 Button Pressed\n");
        EXTI_ClearITPendingBit(GPIO_EXTI_Line_KEY3);
    }
}
/*
static bool toggle_data_key3 = FALSE;
void EXTI3_IRQHandler(void)
{
    DEBUG_MSG_FUNC_START;
    if(EXTI_GetITStatus(GPIO_EXTI_Line_KEY3) != RESET) {
       
        Send_Buffer[0] = 0x08;

        if(toggle_data_key3)
        {
            toggle_data_key3 = FALSE;
            Send_Buffer[1] = 0x01;
            printf("Key3 Button : TRUE\n");
        }
        else
        {
            toggle_data_key3 = TRUE;
            Send_Buffer[1] = 0x00;
            printf("Key3 Button : FALSE\n");
        }

        UserToPMABufferCopy(Send_Buffer, ENDP1_TXADDR, 2);
        SetEPTxCount(ENDP1, 2);
        SetEPTxValid(ENDP1);

        EXTI_ClearITPendingBit(GPIO_EXTI_Line_KEY3);
    }
}
*/
/*******************************************************************************
* Function Name  : EXTI4_IRQHandler
* Description    : This function handles External interrupt Line 4 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI4_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMA1_Channel1_IRQHandler
* Description    : This function handles DMA1 Channel 1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel1_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMA1_Channel2_IRQHandler
* Description    : This function handles DMA1 Channel 2 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel2_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMA1_Channel3_IRQHandler
* Description    : This function handles DMA1 Channel 3 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel3_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMA1_Channel4_IRQHandler
* Description    : This function handles DMA1 Channel 4 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel4_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMA1_Channel5_IRQHandler
* Description    : This function handles DMA1 Channel 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel5_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMA1_Channel6_IRQHandler
* Description    : This function handles DMA1 Channel 6 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel6_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMA1_Channel7_IRQHandler
* Description    : This function handles DMA1 Channel 7 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel7_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : ADC1_2_IRQHandler
* Description    : This function handles ADC1 and ADC2 global interrupts requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC1_2_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : USB_HP_CAN_TX_IRQHandler
* Description    : This function handles USB High Priority or CAN TX interrupts 
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USB_HP_CAN1_TX_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : USB_LP_CAN_RX0_IRQHandler
* Description    : This function handles USB Low Priority or CAN RX0 interrupts 
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USB_LP_CAN1_RX0_IRQHandler(void)
{
   // DEBUG_MSG_FUNC_START;
    //USB_Istr();    
}

/*******************************************************************************
* Function Name  : CAN_RX1_IRQHandler
* Description    : This function handles CAN RX1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN1_RX1_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : CAN_SCE_IRQHandler
* Description    : This function handles CAN SCE interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN1_SCE_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : EXTI9_5_IRQHandler
* Description    : This function handles External lines 9 to 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI9_5_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM1_BRK_IRQHandler
* Description    : This function handles TIM1 Break interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_BRK_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM1_UP_IRQHandler
* Description    : This function handles TIM1 overflow and update interrupt 
*                  request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_UP_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM1_TRG_COM_IRQHandler
* Description    : This function handles TIM1 Trigger and commutation interrupts 
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_TRG_COM_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM1_CC_IRQHandler
* Description    : This function handles TIM1 capture compare interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_CC_IRQHandler(void)
{
}
/*******************************************************************************
* Function Name  : TIM2_IRQHandler
* Description    : This function handles TIM2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM2_IRQHandler(void)
{
  /*Clear TIM2 update interrupt*/
  TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
  //LED0_Toggle();
  msecCounter();
}

/*******************************************************************************
* Function Name  : TIM3_IRQHandler
* Description    : This function handles TIM3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM3_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM4_IRQHandler
* Description    : This function handles TIM4 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM4_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : I2C1_EV_IRQHandler
* Description    : This function handles I2C1 Event interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C1_EV_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : I2C1_ER_IRQHandler
* Description    : This function handles I2C1 Error interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C1_ER_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : I2C2_EV_IRQHandler
* Description    : This function handles I2C2 Event interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C2_EV_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : I2C2_ER_IRQHandler
* Description    : This function handles I2C2 Error interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C2_ER_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : SPI1_IRQHandler
* Description    : This function handles SPI1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI1_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : SPI2_IRQHandler
* Description    : This function handles SPI2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI2_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : USART1_IRQHandler
* Description    : This function handles USART1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART1_IRQHandler(void)
{
////  if(USART_GetITStatus (USART1, USART_IT_RXNE) != RESET){
////    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
////    //Uart1_Push(USART_ReceiveData(USART1));
////    
////    led0_on();
////  }
//  //USART_ClearITPendingBit(USART1, USART_IT_RXNE);
//  
//  USART_ClearITPendingBit(USART1, USART_IT_RXNE);
//  //Uart1_Push(USART_ReceiveData(USART1));
//  enqueue(&buffer, USART_ReceiveData(USART1));
//    
//  LED0_Toggle();
}

/*******************************************************************************
* Function Name  : USART2_IRQHandler
* Description    : This function handles USART2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART2_IRQHandler(void)
{
  USART_ClearITPendingBit(USART2, USART_IT_RXNE);
  //Uart1_Push(USART_ReceiveData(USART1));
  enqueue(&uart2_buffer, USART_ReceiveData(USART2));
  //printf("@\n");
}

/*******************************************************************************
* Function Name  : USART3_IRQHandler
* Description    : This function handles USART3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART3_IRQHandler(void)
{
  USART_ClearITPendingBit(USART3, USART_IT_RXNE);
  //Uart1_Push(USART_ReceiveData(USART1));
  enqueue(&buffer, USART_ReceiveData(USART3));
//  
//  led0_on();
}

/*******************************************************************************
* Function Name  : EXTI15_10_IRQHandler
* Description    : This function handles External lines 15 to 10 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI15_10_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : RTCAlarm_IRQHandler
* Description    : This function handles RTC Alarm interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTCAlarm_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : USBWakeUp_IRQHandler
* Description    : This function handles USB WakeUp interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USBWakeUp_IRQHandler(void)
{
    DEBUG_MSG_FUNC_START;
#if 1
    EXTI_ClearITPendingBit(EXTI_Line18);
#endif
}

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
