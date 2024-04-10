//#define __CORTEXM3__ 

#include "stm32f10x.h"
#include <stdio.h>
#include <string.h>
#include "llio.h"
#include "led.h"
#include "key.h"
#include "systick.h"
#include "my_timer.h"
#include "sensor.h"
#include "circle_queue.h"
#include "wifi_to_uart.h"

#include <stdlib.h>
#include <stdio.h>
#include "dynamixel_sdk.h"                                  // Uses Dynamixel SDK library

// Control table address
//#define ADDR_PRO_TORQUE_ENABLE          64                 // Control table address is different in Dynamixel model
#define ADDR_PRO_TORQUE_ENABLE          24                   //mx-12w
#define ADDR_PRO_GOAL_POSITION          596
#define ADDR_PRO_PRESENT_POSITION       611

// Data Byte Length
#define LEN_PRO_GOAL_POSITION           4
#define LEN_PRO_PRESENT_POSITION        4

// Protocol version
#define PROTOCOL_VERSION                1.0     //1.0, 2.0  // See which protocol version is used in the Dynamixel

// Default setting
#define DXL1_ID                         1                   // Dynamixel#1 ID: 1
#define DXL2_ID                         2                   // Dynamixel#2 ID: 2
#define BAUDRATE                        2000000
#define DEVICENAME                      "UART3"             // Check which port is being used on your controller
                                                            // ex) Windows: "COM1"   Linux: "/dev/ttyUSB0" Mac: "/dev/tty.usbserial-*"

#define TORQUE_ENABLE                   1                   // Value for enabling the torque
#define TORQUE_DISABLE                  0                   // Value for disabling the torque
#define DXL_MINIMUM_POSITION_VALUE      -150000             // Dynamixel will rotate between this value
#define DXL_MAXIMUM_POSITION_VALUE      150000              // and this value (note that the Dynamixel would not move when the position value is out of movable range. Check e-manual about the range of the Dynamixel you use.)
#define DXL_MOVING_STATUS_THRESHOLD     20                  // Dynamixel moving status threshold

//GET SPEED
#define SPEED_STATUS                  38              //2BYTE         mx-12w
//#define SPEED_STATUS                    128             //4BYTE

#define ESC_ASCII_VALUE                 0x1b

//#define SPEED_REGISTER                  104             //4BYTE
#define SPEED_REGISTER                  32              //2BYTE         mx-12w

#define OPERATING_MODE_REGISTER         11              //1BYTE
#define GOAL_PWM_REGISTER               100             //2BYTE

#define SPEED_CONTROL_MODE              1
#define PWM_CONTROL_MODE                16

#define PWM_SPEED_VAL                   885     //PWM
#define SPEED_VAL                       1023//370

Queue buffer, uart2_buffer;

PacketData *packetData;
int     g_used_port_num;
uint8_t    *g_is_using;


int main()
{
  int dxl_comm_result = COMM_TX_FAIL;               // Communication result
  uint8_t dxl_error = 0;                            // Dynamixel error
  int speed_value = 30, r_speed_value = 0, l_speed_value = 0;
  int gain = 10;//140;
//  int speed_value = PWM_SPEED_VAL, r_speed_value = 0, l_speed_value = 0;
//  int gain = 200;
  int error_value = 0, past_error_value = 10;
  
  int i;
  int ch = 0;
  int command_ch = 0;
  char command_str[20];
  int command_index = 0;
  int loss_state = 0;
  char *r_token, *l_token;
  //int str_cnt=0;
  int move_start = 0;
  
  SystemInit();
  led_init();
  llio_init();
  USART2_init();
  NVIC_USART2_Configuration();
  systick_init();
  WIFI_reset_init();
  sensor_init();
  init_circle_queue(&buffer);
  init_circle_queue(&uart2_buffer);
  
  printf("\033[H\033[J");

  // Initialize PortHandler Structs
  // Set the port path
  // Get methods and members of PortHandlerLinux or PortHandlerWindows
  int port_num = portHandler(DEVICENAME);

  // Initialize PacketHandler Structs
  packetHandler();

  // Open port
  if (openPort(port_num))
  {
    printf("Succeeded to open the port!\n");
  }
  else
  {
    printf("Failed to open the port!\n");
    printf("Press any key to terminate...\n");
    //getch();
    return 0;
  }

  // Set port baudrate
  if (setBaudRate(port_num, BAUDRATE))
  {
    printf("Succeeded to change the baudrate!\n");
  }
  else
  {
    printf("Failed to change the baudrate!\n");
    printf("Press any key to terminate...\n");
    //getch();
    return 0;
  }
  
  // Disable Dynamixel#1 Torque
  write1ByteTxRx(port_num, PROTOCOL_VERSION, DXL1_ID, ADDR_PRO_TORQUE_ENABLE, TORQUE_DISABLE);
  if ((dxl_comm_result = getLastTxRxResult(port_num, PROTOCOL_VERSION)) != COMM_SUCCESS)
  {
    printf("%s\n", getTxRxResult(PROTOCOL_VERSION, dxl_comm_result));
  }
  else if ((dxl_error = getLastRxPacketError(port_num, PROTOCOL_VERSION)) != 0)
  {
    printf("%s\n", getRxPacketError(PROTOCOL_VERSION, dxl_error));
  }

  // Disable Dynamixel#2 Torque
  write1ByteTxRx(port_num, PROTOCOL_VERSION, DXL2_ID, ADDR_PRO_TORQUE_ENABLE, TORQUE_DISABLE);
  if ((dxl_comm_result = getLastTxRxResult(port_num, PROTOCOL_VERSION)) != COMM_SUCCESS)
  {
    printf("%s\n", getTxRxResult(PROTOCOL_VERSION, dxl_comm_result));
  }
  else if ((dxl_error = getLastRxPacketError(port_num, PROTOCOL_VERSION)) != 0)
  {
    printf("%s\n", getRxPacketError(PROTOCOL_VERSION, dxl_error));
  }
  
  //Operation Mode setting
  //SPEED CONTROL MODE
//  write1ByteTxRx(port_num, PROTOCOL_VERSION, DXL1_ID, OPERATING_MODE_REGISTER, SPEED_CONTROL_MODE);
//  write1ByteTxRx(port_num, PROTOCOL_VERSION, DXL2_ID, OPERATING_MODE_REGISTER, SPEED_CONTROL_MODE);
  //PWM CONTROL MODE
//  write1ByteTxRx(port_num, PROTOCOL_VERSION, DXL1_ID, OPERATING_MODE_REGISTER, PWM_CONTROL_MODE);
//  write1ByteTxRx(port_num, PROTOCOL_VERSION, DXL2_ID, OPERATING_MODE_REGISTER, PWM_CONTROL_MODE);

  // Enable Dynamixel#1 Torque
  write1ByteTxRx(port_num, PROTOCOL_VERSION, DXL1_ID, ADDR_PRO_TORQUE_ENABLE, TORQUE_ENABLE);
  if ((dxl_comm_result = getLastTxRxResult(port_num, PROTOCOL_VERSION)) != COMM_SUCCESS)
  {
    printf("%s\n", getTxRxResult(PROTOCOL_VERSION, dxl_comm_result));
  }
  else if ((dxl_error = getLastRxPacketError(port_num, PROTOCOL_VERSION)) != 0)
  {
    printf("%s\n", getRxPacketError(PROTOCOL_VERSION, dxl_error));
  }
  else
  {
    printf("Dynamixel#%d has been successfully connected \n", DXL1_ID);
  }

  // Enable Dynamixel#2 Torque
  write1ByteTxRx(port_num, PROTOCOL_VERSION, DXL2_ID, ADDR_PRO_TORQUE_ENABLE, TORQUE_ENABLE);
  if ((dxl_comm_result = getLastTxRxResult(port_num, PROTOCOL_VERSION)) != COMM_SUCCESS)
  {
    printf("%s\n", getTxRxResult(PROTOCOL_VERSION, dxl_comm_result));
  }
  else if ((dxl_error = getLastRxPacketError(port_num, PROTOCOL_VERSION)) != 0)
  {
    printf("%s\n", getRxPacketError(PROTOCOL_VERSION, dxl_error));
  }
  else
  {
    printf("Dynamixel#%d has been successfully connected \n", DXL2_ID);
  }
  
//  // 1. wifi통신
//  while(1){
//    ch = MyLowLevelGetchar_Uart2();
//    if(ch == 'w'){
//      write2ByteTxRx(port_num, PROTOCOL_VERSION, DXL1_ID, SPEED_REGISTER, 100);
//      write2ByteTxRx(port_num, PROTOCOL_VERSION, DXL2_ID, SPEED_REGISTER, 100+1024);
//    }
//    else if(ch == 's'){
//      write4ByteTxRx(port_num, PROTOCOL_VERSION, DXL1_ID, SPEED_REGISTER, -400);
//      write4ByteTxRx(port_num, PROTOCOL_VERSION, DXL2_ID, SPEED_REGISTER, 400);
//    }
//    else if(ch == 'a'){
//      write4ByteTxRx(port_num, PROTOCOL_VERSION, DXL1_ID, SPEED_REGISTER, 100);
//      write4ByteTxRx(port_num, PROTOCOL_VERSION, DXL2_ID, SPEED_REGISTER, -400);
//    }
//    else if(ch == 'd'){
//      write4ByteTxRx(port_num, PROTOCOL_VERSION, DXL1_ID, SPEED_REGISTER, 400);
//      write4ByteTxRx(port_num, PROTOCOL_VERSION, DXL2_ID, SPEED_REGISTER, -100);
//    }
//    else if(ch == 'x'){
//      write4ByteTxRx(port_num, PROTOCOL_VERSION, DXL1_ID, SPEED_REGISTER, 0);
//      write4ByteTxRx(port_num, PROTOCOL_VERSION, DXL2_ID, SPEED_REGISTER, 0);
//    }
//  }
  
  
  
  //버퍼 비우기
  while(1){
    if(is_empty(&uart2_buffer))
      continue;
    
    if(peek(&uart2_buffer) == 0)
      dequeue(&uart2_buffer);
      //printf("%d\n", dequeue(&uart2_buffer));
    
    break;
  }
  
  // 2. 라인트레이서
  while (1)
  {      
    //str_cnt = 0;
//    while(1){
//      command_ch = MyLowLevelGetchar_Uart2();
//      command_str[str_cnt] = command_ch;
//      if(command_str[0] != '?')
//        continue;
//      if(command_str[1] == '?' || command_str[1] == '!'){
//        str_cnt = 0;
//        continue;
//      }
//      str_cnt++;
//      if(command_ch == '!'){
//        break;
//      }
//    }
    
    //-------------------------------------------------
//    int rx_length;
//
//    rx_length = queue_available(&buffer);
//  
//    if (rx_length > length)
//      rx_length = length;
//  
//    for (int i = 0; i < rx_length; i++)
//    {
//      packet[i] = dequeue(&buffer);
//    }
//  
//    return rx_length;
    
    //-------------------------------------------------
    if(is_empty(&uart2_buffer))
      continue;
    
    //printf("&\n", command_str[command_index]);
    
    command_index = 0;
    while(1){
      if(!is_empty(&uart2_buffer)){
      command_str[command_index] = dequeue(&uart2_buffer);
      printf("%c\n", command_str[command_index]);
      printf("<%d>\n", command_index);
      if(command_str[command_index] == '!')
        break;

      command_index++;
      }
    }
//    if(loss_state){
//      loss_state = 0;
//      continue;
//    }
    command_str[command_index + 1] = '\0';
    
    printf("%s\n", command_str);
    
    r_token = strtok(command_str, ",");
    l_token = strtok(NULL, ",");
    
    l_token[strlen(l_token) - 1] = '\0';
    
    sscanf(r_token, "%d", &r_speed_value);
    sscanf(l_token, "%d", &l_speed_value);
    
    //printf("r : %d, l : %d\n", r_speed_value, l_speed_value);
    
    l_speed_value = (l_speed_value < 0) ? 1024 - l_speed_value : l_speed_value;
    r_speed_value = (r_speed_value < 0) ? -r_speed_value : r_speed_value + 1024;
    
    write2ByteTxRx(port_num, PROTOCOL_VERSION, DXL1_ID, SPEED_REGISTER, l_speed_value);
    if ((dxl_comm_result = getLastTxRxResult(port_num, PROTOCOL_VERSION)) != COMM_SUCCESS)
    {
      printf("%s\n", getTxRxResult(PROTOCOL_VERSION, dxl_comm_result));
    }
    else if ((dxl_error = getLastRxPacketError(port_num, PROTOCOL_VERSION)) != 0)
    {
      printf("%s\n", getRxPacketError(PROTOCOL_VERSION, dxl_error));
    }
    
    write2ByteTxRx(port_num, PROTOCOL_VERSION, DXL2_ID, SPEED_REGISTER, r_speed_value);
    if ((dxl_comm_result = getLastTxRxResult(port_num, PROTOCOL_VERSION)) != COMM_SUCCESS)
    {
      printf("%s\n", getTxRxResult(PROTOCOL_VERSION, dxl_comm_result));
    }
    else if ((dxl_error = getLastRxPacketError(port_num, PROTOCOL_VERSION)) != 0)
    {
      printf("%s\n", getRxPacketError(PROTOCOL_VERSION, dxl_error));
    }
    
    
//    command_ch = MyLowLevelGetchar_Uart2();
//    printf("%c\n",command_ch);
//    if(command_ch == 'r'){
//      write2ByteTxRx(port_num, PROTOCOL_VERSION, DXL1_ID, SPEED_REGISTER, 15 + 1024);
//      write2ByteTxRx(port_num, PROTOCOL_VERSION, DXL2_ID, SPEED_REGISTER, 15 + 1024);
//    }
//    else if(command_ch == 'c'){
//      write2ByteTxRx(port_num, PROTOCOL_VERSION, DXL1_ID, SPEED_REGISTER, 15);
//      write2ByteTxRx(port_num, PROTOCOL_VERSION, DXL2_ID, SPEED_REGISTER, 15);
//    }
//    else if(command_ch == 's'){
//      write2ByteTxRx(port_num, PROTOCOL_VERSION, DXL1_ID, SPEED_REGISTER, 0);
//      write2ByteTxRx(port_num, PROTOCOL_VERSION, DXL2_ID, SPEED_REGISTER, 0);
//      move_start = 1;
//      continue;
//    }
//    else if(command_ch == 'f'){
//      write2ByteTxRx(port_num, PROTOCOL_VERSION, DXL1_ID, SPEED_REGISTER, 0);
//      write2ByteTxRx(port_num, PROTOCOL_VERSION, DXL2_ID, SPEED_REGISTER, 0);
//      move_start = 0;
//      continue;
//    }
//    else{
//      if((command_ch - '0') < 5)
//        error_value = command_ch - '0';
//      else
//        error_value = -(command_ch - '0' - 4);
//    }
//    
//    if(move_start){
//      r_speed_value = speed_value - gain * error_value;
//      l_speed_value = speed_value + gain * error_value;
//      
////        if(r_speed_value > SPEED_VAL)
////          r_speed_value = SPEED_VAL;
////        else if(r_speed_value < -SPEED_VAL)
////          r_speed_value = -SPEED_VAL;
////        
////        if(l_speed_value > SPEED_VAL)
////          l_speed_value = SPEED_VAL;
////        else if(l_speed_value < -SPEED_VAL)
////          l_speed_value = -SPEED_VAL;
//      
//      if(r_speed_value > SPEED_VAL)
//        r_speed_value = SPEED_VAL;
//      else if(r_speed_value < -SPEED_VAL)
//        r_speed_value = -SPEED_VAL;
//      
//      if(l_speed_value > SPEED_VAL)
//        l_speed_value = SPEED_VAL;
//      else if(l_speed_value < -SPEED_VAL)
//        l_speed_value = -SPEED_VAL;
//      
////        if(r_speed_value > PWM_SPEED_VAL)
////          r_speed_value = PWM_SPEED_VAL;
////        else if(r_speed_value < -PWM_SPEED_VAL)
////          r_speed_value = -PWM_SPEED_VAL;
////        
////        if(l_speed_value > PWM_SPEED_VAL)
////          l_speed_value = PWM_SPEED_VAL;
////        else if(l_speed_value < -PWM_SPEED_VAL)
////          l_speed_value = -PWM_SPEED_VAL;
//      
//      //printf("r : %d, l : %d\n", r_speed_value, l_speed_value);
//      
//      l_speed_value = (l_speed_value < 0) ? 1024 - l_speed_value : l_speed_value;
//      r_speed_value = (r_speed_value < 0) ? -r_speed_value : r_speed_value + 1024;
//  
//      if(error_value != past_error_value){
//        //SPEED CONTROL MODE
////          write4ByteTxRx(port_num, PROTOCOL_VERSION, DXL1_ID, SPEED_REGISTER, l_speed_value);
////          if ((dxl_comm_result = getLastTxRxResult(port_num, PROTOCOL_VERSION)) != COMM_SUCCESS)
////          {
////            printf("%s\n", getTxRxResult(PROTOCOL_VERSION, dxl_comm_result));
////          }
////          else if ((dxl_error = getLastRxPacketError(port_num, PROTOCOL_VERSION)) != 0)
////          {
////            printf("%s\n", getRxPacketError(PROTOCOL_VERSION, dxl_error));
////          }
////          write4ByteTxRx(port_num, PROTOCOL_VERSION, DXL2_ID, SPEED_REGISTER, -r_speed_value);
////          if ((dxl_comm_result = getLastTxRxResult(port_num, PROTOCOL_VERSION)) != COMM_SUCCESS)
////          {
////            printf("%s\n", getTxRxResult(PROTOCOL_VERSION, dxl_comm_result));
////          }
////          else if ((dxl_error = getLastRxPacketError(port_num, PROTOCOL_VERSION)) != 0)
////          {
////            printf("%s\n", getRxPacketError(PROTOCOL_VERSION, dxl_error));
////          }
//        //SPEED CONTROL MODE (mx-12w)
//        write2ByteTxRx(port_num, PROTOCOL_VERSION, DXL1_ID, SPEED_REGISTER, l_speed_value);
//        if ((dxl_comm_result = getLastTxRxResult(port_num, PROTOCOL_VERSION)) != COMM_SUCCESS)
//        {
//          printf("%s\n", getTxRxResult(PROTOCOL_VERSION, dxl_comm_result));
//        }
//        else if ((dxl_error = getLastRxPacketError(port_num, PROTOCOL_VERSION)) != 0)
//        {
//          printf("%s\n", getRxPacketError(PROTOCOL_VERSION, dxl_error));
//        }
//        
//        write2ByteTxRx(port_num, PROTOCOL_VERSION, DXL2_ID, SPEED_REGISTER, r_speed_value);
//        if ((dxl_comm_result = getLastTxRxResult(port_num, PROTOCOL_VERSION)) != COMM_SUCCESS)
//        {
//          printf("%s\n", getTxRxResult(PROTOCOL_VERSION, dxl_comm_result));
//        }
//        else if ((dxl_error = getLastRxPacketError(port_num, PROTOCOL_VERSION)) != 0)
//        {
//          printf("%s\n", getRxPacketError(PROTOCOL_VERSION, dxl_error));
//        }
//        
//        //PWM CONTROL MODE
////          write2ByteTxRx(port_num, PROTOCOL_VERSION, DXL1_ID, GOAL_PWM_REGISTER, l_speed_value);
////          if ((dxl_comm_result = getLastTxRxResult(port_num, PROTOCOL_VERSION)) != COMM_SUCCESS)
////          {
////            printf("%s\n", getTxRxResult(PROTOCOL_VERSION, dxl_comm_result));
////          }
////          else if ((dxl_error = getLastRxPacketError(port_num, PROTOCOL_VERSION)) != 0)
////          {
////            printf("%s\n", getRxPacketError(PROTOCOL_VERSION, dxl_error));
////          }
////          write2ByteTxRx(port_num, PROTOCOL_VERSION, DXL2_ID, GOAL_PWM_REGISTER, -r_speed_value);
////          if ((dxl_comm_result = getLastTxRxResult(port_num, PROTOCOL_VERSION)) != COMM_SUCCESS)
////          {
////            printf("%s\n", getTxRxResult(PROTOCOL_VERSION, dxl_comm_result));
////          }
////          else if ((dxl_error = getLastRxPacketError(port_num, PROTOCOL_VERSION)) != 0)
////          {
////            printf("%s\n", getRxPacketError(PROTOCOL_VERSION, dxl_error));
////          }
//        
////          write2ByteTxRx(port_num, PROTOCOL_VERSION, DXL1_ID, GOAL_PWM_REGISTER, 800);
////          write2ByteTxRx(port_num, PROTOCOL_VERSION, DXL2_ID, GOAL_PWM_REGISTER, -800);
//        //printf("!!\n");
//      }
//      
////        write2ByteTxRx(port_num, PROTOCOL_VERSION, DXL1_ID, SPEED_REGISTER, 20);
////        if ((dxl_comm_result = getLastTxRxResult(port_num, PROTOCOL_VERSION)) != COMM_SUCCESS)
////        {
////          printf("%s\n", getTxRxResult(PROTOCOL_VERSION, dxl_comm_result));
////        }
////        else if ((dxl_error = getLastRxPacketError(port_num, PROTOCOL_VERSION)) != 0)
////        {
////          printf("%s\n", getRxPacketError(PROTOCOL_VERSION, dxl_error));
////        }
////        
////        write2ByteTxRx(port_num, PROTOCOL_VERSION, DXL2_ID, SPEED_REGISTER, 20);
////        if ((dxl_comm_result = getLastTxRxResult(port_num, PROTOCOL_VERSION)) != COMM_SUCCESS)
////        {
////          printf("%s\n", getTxRxResult(PROTOCOL_VERSION, dxl_comm_result));
////        }
////        else if ((dxl_error = getLastRxPacketError(port_num, PROTOCOL_VERSION)) != 0)
////        {
////          printf("%s\n", getRxPacketError(PROTOCOL_VERSION, dxl_error));
////        }
//      
//      //현재 속도 출력
////        printf("motor1_speed : %d\n", read2ByteTxRx(port_num, PROTOCOL_VERSION, DXL1_ID, SPEED_STATUS));
////        printf("motor2_speed : %d\n", read2ByteTxRx(port_num, PROTOCOL_VERSION, DXL2_ID, SPEED_STATUS) - 1024);
//      
//      past_error_value = error_value;
//      //printf("??\n");
//      delay_ms(1);
//    }
  }

  // Disable Dynamixel#1 Torque
  write1ByteTxRx(port_num, PROTOCOL_VERSION, DXL1_ID, ADDR_PRO_TORQUE_ENABLE, TORQUE_DISABLE);
  if ((dxl_comm_result = getLastTxRxResult(port_num, PROTOCOL_VERSION)) != COMM_SUCCESS)
  {
    printf("%s\n", getTxRxResult(PROTOCOL_VERSION, dxl_comm_result));
  }
  else if ((dxl_error = getLastRxPacketError(port_num, PROTOCOL_VERSION)) != 0)
  {
    printf("%s\n", getRxPacketError(PROTOCOL_VERSION, dxl_error));
  }

  // Disable Dynamixel#2 Torque
  write1ByteTxRx(port_num, PROTOCOL_VERSION, DXL2_ID, ADDR_PRO_TORQUE_ENABLE, TORQUE_DISABLE);
  if ((dxl_comm_result = getLastTxRxResult(port_num, PROTOCOL_VERSION)) != COMM_SUCCESS)
  {
    printf("%s\n", getTxRxResult(PROTOCOL_VERSION, dxl_comm_result));
  }
  else if ((dxl_error = getLastRxPacketError(port_num, PROTOCOL_VERSION)) != 0)
  {
    printf("%s\n", getRxPacketError(PROTOCOL_VERSION, dxl_error));
  }

  // Close port
  closePort(port_num);
  
  return 0;
}


































































