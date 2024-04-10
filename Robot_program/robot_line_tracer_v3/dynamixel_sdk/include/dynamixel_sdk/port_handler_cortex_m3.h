/*******************************************************************************
* Copyright 2017 ROBOTIS CO., LTD.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

/* Author: Ryu Woon Jung (Leon) */

#ifndef DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_CORTEXM3_PORTHANDLERCORTEXM3_C_H_
#define DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_CORTEXM3_PORTHANDLERCORTEXM3_C_H_

#include "stm32f10x.h"

#include "port_handler.h"

void PB5_init();
void PB5_on();
void PB5_off();

int portHandlerCortexm3            (const char *port_name);

uint8_t setupPortCortexm3          (int port_num, const int cflag_baud);
uint8_t setCustomBaudrateCortexm3  (int port_num, int speed);
int     getCFlagBaud            (const int baudrate);

double  getCurrentTimeCortexm3     ();
double  getTimeSinceStartCortexm3  (int port_num);

uint8_t openPortCortexm3           (int port_num);
void    closePortCortexm3          (int port_num);
void    clearPortCortexm3          (int port_num);

void    setPortNameCortexm3        (int port_num, const char *port_name);
char   *getPortNameCortexm3        (int port_num);

uint8_t setBaudRateCortexm3        (int port_num, const int baudrate);
int     getBaudRateCortexm3        (int port_num);

int     getBytesAvailableCortexm3  (int port_num);

int     readPortCortexm3           (int port_num, uint8_t *packet, int length);
int     writePortCortexm3          (int port_num, uint8_t *packet, int length);

void    setPacketTimeoutCortexm3     (int port_num, uint16_t packet_length);
void    setPacketTimeoutMSecCortexm3 (int port_num, double msec);
uint8_t isPacketTimeoutCortexm3      (int port_num);

#endif /* DYNAMIXEL_SDK_INCLUDE_DYNAMIXEL_SDK_CORTEXM3_PORTHANDLERCORTEXM3_C_H_ */
