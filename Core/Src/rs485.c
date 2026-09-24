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

volatile uint8_t U3_Received = 0; // Flaga odebrania danych
volatile uint16_t U3_size_RX = 0;  // Długość odebranej ramki
volatile uint16_t U3_size_TX = 0;  // Długość nadawanej ramki
uint8_t U3_RxBuffer[256];
uint8_t U3_TxBuffer[256];

#define PLC_ADDRESS     0xE1

#define CMD_READ_IO     0x0A
#define CMD_WRITE_IO    0x0B


void init_rx_rs485(void)
{
	HAL_UARTEx_ReceiveToIdle_DMA(&huart3, U3_RxBuffer, 256);
}
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
}*/
static void RS485_SendStatus(void)
{
    uint16_t crc;

    /*
     * Odpowiedź:
     *
     * [0] adres
     * [1] IN  00..07
     * [2] IN  10..17
     * [3] IN  20..27
     * [4] OUT 00..07
     * [5] OUT 10..17
     * [6] OUT 20..27
     * [7..10] CRC ASCII
     */

    U3_size_TX = 0;

    U3_TxBuffer[U3_size_TX++] = PLC_ADDRESS;

    U3_TxBuffer[U3_size_TX++] = IO_GetInputGroup(0);
    U3_TxBuffer[U3_size_TX++] = IO_GetInputGroup(1);
    U3_TxBuffer[U3_size_TX++] = IO_GetInputGroup(2);

    U3_TxBuffer[U3_size_TX++] = IO_GetOutputGroup(0);
    U3_TxBuffer[U3_size_TX++] = IO_GetOutputGroup(1);
    U3_TxBuffer[U3_size_TX++] = IO_GetOutputGroup(2);


    crc = mmodbus_crc16(U3_TxBuffer, U3_size_TX);

    U3_size_TX += sprintf(
        (char *)&U3_TxBuffer[U3_size_TX],
        "%04X",
        crc
    );


    /* RS485 -> nadawanie */
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


static uint8_t RS485_CheckCRC(void)
{
    uint16_t crc;
    char s_crc[5];

    if (U3_size_RX < 6)
        return 0;

    /*
     * Ostatnie 4 bajty ramki to CRC zapisane jako ASCII HEX.
     */
    crc = mmodbus_crc16(
        U3_RxBuffer,
        U3_size_RX - 4
    );

    sprintf(s_crc, "%04X", crc);

    if ((U3_RxBuffer[U3_size_RX - 4] == (uint8_t)s_crc[0]) &&
        (U3_RxBuffer[U3_size_RX - 3] == (uint8_t)s_crc[1]) &&
        (U3_RxBuffer[U3_size_RX - 2] == (uint8_t)s_crc[2]) &&
        (U3_RxBuffer[U3_size_RX - 1] == (uint8_t)s_crc[3]))
    {
        return 1;
    }

    return 0;
}


void rs485(void)
{
    uint8_t command;

    if (!U3_Received)
        return;

    U3_Received = 0;


    /*
     * Minimum:
     *
     * adres
     * funkcja
     * CRC = 4 bajty ASCII
     *
     * razem 6 bajtów
     */
    if (U3_size_RX < 6)
        return;


    /* Czy ramka jest do tego sterownika? */
    if (U3_RxBuffer[0] != PLC_ADDRESS)
        return;


    /* Kontrola CRC */
    if (!RS485_CheckCRC())
        return;


    command = U3_RxBuffer[1];


    switch (command)
    {
        /* =============================================
         * 0A - tylko odczyt IN/OUT
         *
         * M -> S:
         * E1 0A CRC
         * ============================================= */
        case CMD_READ_IO:

            /*
             * Dokładnie:
             * adres + funkcja + CRC
             */
            if (U3_size_RX != 6)
                return;

            RS485_SendStatus();

            break;


        /* =============================================
         * 0B - ustaw OUT i odeślij aktualny stan
         *
         * M -> S:
         *
         * [0] E1
         * [1] 0B
         * [2] OUT 00..07
         * [3] OUT 10..17
         * [4] OUT 20..27
         * [5..8] CRC
         *
         * razem 9 bajtów
         * ============================================= */
        case CMD_WRITE_IO:

            if (U3_size_RX != 9)
                return;


            IO_SetOutputGroup(0, U3_RxBuffer[2]);
            IO_SetOutputGroup(1, U3_RxBuffer[3]);
            IO_SetOutputGroup(2, U3_RxBuffer[4]);


            /*
             * Fizycznie ustawiamy przekaźniki od razu.
             */
            IO_WriteOutputs();


            /*
             * I odsyłamy aktualny IN + OUT.
             */
            RS485_SendStatus();

            break;


        default:
            /* Nieznana funkcja */
            break;
    }
}
