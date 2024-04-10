#include "my_timer.h"

static volatile uint32_t msecCount = 0;
static volatile uint32_t secCount = 0;

void RCC_Configuration(uint32_t RCC_APB1Periph_TIMx){
  /*Enable or Disable TIMx clock*/
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIMx, ENABLE);
}

void NVIC_Configuration(IRQn_Type TIMx_IRQn, int PreemptionPriority_val, int SubPriority_val){
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /*Set the Vector Table base location at 0x08000000*/
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
  
  /*Configure one bit for preemption priority*/
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  
  /*Enable the TIMx Interrupt*/
  NVIC_InitStructure.NVIC_IRQChannel = TIMx_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PreemptionPriority_val;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = SubPriority_val;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void TIM_Configuration(uint16_t TIM_Period_val, uint16_t TIM_Prescaler_val, TIM_TypeDef* TIMx){
  TIM_TimeBaseInitTypeDef TIM_TimebaseStructure;
  
  /*TIMx configuration*/
  TIM_TimebaseStructure.TIM_Period = TIM_Period_val;        //0x4AF;
  TIM_TimebaseStructure.TIM_Prescaler = TIM_Prescaler_val;      //0xEA5F;
  TIM_TimebaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;       //0x0
  TIM_TimebaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //0x0
  TIM_TimebaseStructure.TIM_RepetitionCounter = 0x0000;
  TIM_TimeBaseInit(TIMx, &TIM_TimebaseStructure);
  
  /*TIMx enable counter*/
  TIM_Cmd(TIMx, ENABLE);
  
  /*Immediate load of TIMx Prescaler value*/
  TIM_PrescalerConfig(TIMx, TIM_Prescaler_val, TIM_PSCReloadMode_Immediate);
  
  /*Clear TIMx update pending flag*/
  TIM_ClearFlag(TIMx, TIM_FLAG_Update);
  
  /*Enable TIMx Update interrupt*/
  TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);
}

void Timer_1_msec_Init(){
  RCC_Configuration(RCC_APB1Periph_TIM2);
  NVIC_Configuration(TIM2_IRQn, 0, 0);
  TIM_Configuration(1, 0x8CA0, TIM2);
}

void msecCounter(void)
{
    msecCount++;

    if(msecCount == 1000)   
    {
      secCount++;
      msecCount = 0;
      //LED0_Toggle();
    }
}

int my_clock_gettime(struct my_timespec *tp){
  tp->tv_sec = secCount;
  tp->tv_msec = msecCount;
  
  return 0;
}
























      
      
        
    
















