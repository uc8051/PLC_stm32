/*
 * rs485.c
 *
 *  Created on: Sep 23, 2026
 *      Author: maswi
 */
#include "rs485.h"
#include "main.h"
#include "io.h"
#include "modbus.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
static void RS485_RestartRX(void);
volatile uint8_t U3_Received = 0; // Flaga odebrania danych
volatile uint16_t U3_size_RX = 0;  // Długość odebranej ramki
volatile uint16_t U3_size_TX = 0;  // Długość nadawanej ramki
volatile uint32_t U3_last_good_rx = 0;
volatile uint8_t U3_force_restart = 0;
uint8_t U3_RxBuffer[256];
uint8_t U3_TxBuffer[256];

#define PLC_ADDRESS     0xE1

#define CMD_READ_IO     0x0A
#define CMD_WRITE_IO    0x0B




/*void rs485(void)
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
*/
static void RS485_RestartRX(void)
{
    U3_size_RX = 0;
    U3_Received = 0;

    HAL_GPIO_WritePin(
        rs485_GPIO_Port,
        rs485_Pin,
        GPIO_PIN_RESET
    );

    HAL_UART_AbortReceive(&huart3);

    __HAL_UART_CLEAR_OREFLAG(&huart3);
    __HAL_UART_CLEAR_NEFLAG(&huart3);
    __HAL_UART_CLEAR_FEFLAG(&huart3);
    __HAL_UART_CLEAR_PEFLAG(&huart3);

    U3_size_RX = 0;
    U3_Received = 0;


    HAL_UARTEx_ReceiveToIdle_DMA(
        &huart3,
        U3_RxBuffer,
        sizeof(U3_RxBuffer)
    );
    //U3_last_good_rx = HAL_GetTick();
}

void init_rx_rs485(void)
{
	 U3_last_good_rx = HAL_GetTick();

	 RS485_RestartRX();

}




static int8_t HexNibble(uint8_t c)
{
    if (c >= '0' && c <= '9')
        return (int8_t)(c - '0');

    if (c >= 'A' && c <= 'F')
        return (int8_t)(c - 'A' + 10);

    if (c >= 'a' && c <= 'f')
        return (int8_t)(c - 'a' + 10);

    return -1;
}


static uint8_t HexByte(uint8_t hi, uint8_t lo, uint8_t *value)
{
    int8_t h;
    int8_t l;

    h = HexNibble(hi);
    l = HexNibble(lo);

    if (h < 0 || l < 0)
        return 0;

    *value = (uint8_t)(((uint8_t)h << 4) | (uint8_t)l);

    return 1;
}


static uint8_t RS485_CheckCRC(void)
{
    uint16_t crc;
    char s_crc[5];

    if (U3_size_RX < 5)
        return 0;

    crc = mmodbus_crc16(
        U3_RxBuffer,
        U3_size_RX - 4
    );

    sprintf(s_crc, "%04X", crc);

    if (U3_RxBuffer[U3_size_RX - 4] != (uint8_t)s_crc[0])
        return 0;

    if (U3_RxBuffer[U3_size_RX - 3] != (uint8_t)s_crc[1])
        return 0;

    if (U3_RxBuffer[U3_size_RX - 2] != (uint8_t)s_crc[2])
        return 0;

    if (U3_RxBuffer[U3_size_RX - 1] != (uint8_t)s_crc[3])
        return 0;

    return 1;
}


static void RS485_SendStatus(void)
{
    uint16_t crc;

    U3_size_TX = 0;

    U3_size_TX += sprintf(
        (char *)&U3_TxBuffer[U3_size_TX],
        "E1"
        "%02X"
        "%02X"
        "%02X"
        "%02X"
        "%02X"
        "%02X",
        IO_GetInputGroup(0),
        IO_GetInputGroup(1),
        IO_GetInputGroup(2),
        IO_GetOutputGroup(0),
        IO_GetOutputGroup(1),
        IO_GetOutputGroup(2)
    );

    crc = mmodbus_crc16(
        U3_TxBuffer,
        U3_size_TX
    );

    U3_size_TX += sprintf(
        (char *)&U3_TxBuffer[U3_size_TX],
        "%04X",
        crc
    );

    HAL_GPIO_WritePin(
        rs485_GPIO_Port,
        rs485_Pin,
        GPIO_PIN_SET
    );

    HAL_UART_Transmit_DMA(
        &huart3,
        U3_TxBuffer,
        U3_size_TX
    );
}


void rs485(void)
{
    uint8_t out0;
    uint8_t out1;
    uint8_t out2;

    if (U3_Received == 0)
        return;

    U3_Received = 0;


    // Minimalna poprawna ramka:
    // E10A + 4 znaki CRC = 8 znakow

    if (U3_size_RX < 8)
    {
        RS485_RestartRX();
        return;
    }


    // Adres E1

    if (U3_RxBuffer[0] != 'E')
    {
        RS485_RestartRX();
        return;
    }

    if (U3_RxBuffer[1] != '1')
    {
        RS485_RestartRX();
        return;
    }


    // CRC

    if (!RS485_CheckCRC())
    {
        RS485_RestartRX();
        return;
    }
    U3_last_good_rx = HAL_GetTick();

    // Funkcja 0A
    // Odczyt wszystkich wejsc i wyjsc
    //
    // Zapytanie:
    // E10A + CRC
    //
    // Przyklad:
    // E10AD790

    if (U3_RxBuffer[2] == '0' &&
        U3_RxBuffer[3] == 'A')
    {
        if (U3_size_RX != 8)
        {
            RS485_RestartRX();
            return;
        }

        RS485_SendStatus();

        return;
    }


    // Funkcja 0B
    // Ustawienie wyjsc i odeslanie aktualnego stanu
    //
    // Format:
    // E10BFFFFFF + CRC
    //
    // OUT 00..07 = FF
    // OUT 10..17 = FF
    // OUT 20..27 = FF

    if (U3_RxBuffer[2] == '0' &&
        U3_RxBuffer[3] == 'B')
    {
        if (U3_size_RX != 14)
        {
            RS485_RestartRX();
            return;
        }

        if (!HexByte(
                U3_RxBuffer[4],
                U3_RxBuffer[5],
                &out0))
        {
            RS485_RestartRX();
            return;
        }

        if (!HexByte(
                U3_RxBuffer[6],
                U3_RxBuffer[7],
                &out1))
        {
            RS485_RestartRX();
            return;
        }

        if (!HexByte(
                U3_RxBuffer[8],
                U3_RxBuffer[9],
                &out2))
        {
            RS485_RestartRX();
            return;
        }

        IO_SetOutputGroup(0, out0);
        IO_SetOutputGroup(1, out1);
        IO_SetOutputGroup(2, out2);

        IO_WriteOutputs();

        RS485_SendStatus();

        return;
    }


    // Nieznana funkcja

    RS485_RestartRX();
}
void rs485_watchdog(void)
{
    if (U3_force_restart)
    {
        U3_force_restart = 0;
        RS485_RestartRX();
        return;
    }

    if ((HAL_GetTick() - U3_last_good_rx) >= 200)
    {
        RS485_RestartRX();
    }
}
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART3)
    {
        U3_force_restart = 1;
    }
}
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART3)
    {
        U3_size_RX = Size;
        U3_Received = 1;
    }
}
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART3)
    {
        HAL_GPIO_WritePin(
            rs485_GPIO_Port,
            rs485_Pin,
            GPIO_PIN_RESET
        );

        RS485_RestartRX();
    }
}
