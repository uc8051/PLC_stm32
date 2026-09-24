/*
 * rs485.c
 *
 *  Created on: Sep 23, 2026
 *      Author: maswi
 */
#include "rs485.h"
#include "main.h"


volatile uint8_t U3_Received = 0; // Flaga odebrania danych
volatile uint16_t U3_size_RX = 0;  // Długość odebranej ramki
volatile uint16_t U3_size_TX = 0;  // Długość nadawanej ramki
uint8_t U3_RxBuffer[256];
uint8_t U3_TxBuffer[256];

void init_rx_rs485(void)
{
	HAL_UARTEx_ReceiveToIdle_DMA(&huart3, U3_RxBuffer, 256);
}
void rs485(void)
{
if (U3_Received == 1)
		    { 	uint16_t  crc;
		    	unsigned char s_crc[10];
		        U3_Received = 0; // Kasowanie flagi


		       if(U3_size_RX > 4)
		       {
		    	   memcpy(U3_TxBuffer, U3_RxBuffer,U3_size_RX);
	               crc = mmodbus_crc16(U3_RxBuffer, U3_size_RX-4);
	               sprintf((char*)s_crc,"%04X",crc);
	               if(U3_RxBuffer[U3_size_RX-1] == s_crc[3] && U3_RxBuffer[U3_size_RX-2] == s_crc[2] && U3_RxBuffer[U3_size_RX-3] == s_crc[1] && U3_RxBuffer[U3_size_RX-4] == s_crc[0])
	                  { // dobra ramka
	            	   HAL_GPIO_WritePin(rs485_GPIO_Port, rs485_Pin, GPIO_PIN_SET);
	            	   U3_size_TX = 0;
	            	   U3_size_TX += sprintf(&U3_TxBuffer,"OK");
	            	   HAL_UART_Transmit_DMA(&huart3, U3_TxBuffer, U3_size_TX);

                      }
	               else
	               {
	            	   HAL_GPIO_WritePin(rs485_GPIO_Port, rs485_Pin, GPIO_PIN_SET);
	            	   U3_size_TX = 0;
	        	   U3_size_TX += sprintf(&U3_TxBuffer,"suma");
	        	   HAL_UART_Transmit_DMA(&huart3, U3_TxBuffer, U3_size_TX);

	               }
		       }
		       else
		       {HAL_GPIO_WritePin(rs485_GPIO_Port, rs485_Pin, GPIO_PIN_SET);
		    	   U3_size_TX = 0;
        	   U3_size_TX += sprintf(&U3_TxBuffer,"Erorr");
        	   HAL_UART_Transmit_DMA(&huart3, U3_TxBuffer, U3_size_TX);

		       }


		    }
}
