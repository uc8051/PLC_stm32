/*
 * rs485.h
 *
 *  Created on: Sep 23, 2026
 *      Author: maswi
 */

#ifndef INC_RS485_H_
#define INC_RS485_H_

#include "main.h"
#include <stdint.h>

extern UART_HandleTypeDef huart3;

extern volatile uint8_t  U3_Received;
extern volatile uint16_t U3_size_RX;
extern volatile uint16_t U3_size_TX;

extern uint8_t U3_RxBuffer[256];
extern uint8_t U3_TxBuffer[256];

void init_rx_rs485(void);
void rs485(void);

#endif /* INC_RS485_H_ */
