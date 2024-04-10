#include "sensor.h"

void sensor_init(){
  GPIO_InitTypeDef GPIO_InitStructure;
    
  /* Enable GPIOA clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_SENSOR, ENABLE);

  /* Configure sensor port */
  GPIO_InitStructure.GPIO_Pin  = GPIO_SENSOR0_PIN | GPIO_SENSOR1_PIN | GPIO_SENSOR2_PIN | GPIO_SENSOR3_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIO_SENSOR, &GPIO_InitStructure);
}

int get_sensor(){
  uint8_t value = 0x0;
  
  if(GPIO_ReadInputDataBit(GPIO_SENSOR, GPIO_SENSOR0_PIN) == Bit_SET)
    value += 0x1;
  if(GPIO_ReadInputDataBit(GPIO_SENSOR, GPIO_SENSOR1_PIN) == Bit_SET)
    value += 0x2;
  if(GPIO_ReadInputDataBit(GPIO_SENSOR, GPIO_SENSOR2_PIN) == Bit_SET)
    value += 0x4;
  if(GPIO_ReadInputDataBit(GPIO_SENSOR, GPIO_SENSOR3_PIN) == Bit_SET)
    value += 0x8;
  
  return value;
}

int get_error(int value){
  int r_errorValue = 0 , l_errorValue = 0, errorValue;
  
  switch(value >> 2){
  case 0x3:
    r_errorValue = 0;
    break;
  case 0x2:
    r_errorValue = 1;
    break;
  case 0x0:
    r_errorValue = 2;
    break;
  case 0x1:
    r_errorValue = 3;
    break;
  default:
    break;
  }
  switch(value & 0x03){
  case 0x3:
    l_errorValue = 0;
    break;
  case 0x2:
    l_errorValue = -3;
    break;
  case 0x0:
    l_errorValue = -2;
    break;
  case 0x1:
    l_errorValue = -1;
    break;
  default:
    break;
  }
  
  errorValue = r_errorValue + l_errorValue;
  
  return errorValue;
}


















