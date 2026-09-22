/*
 * modbus.h
 *
 *  Created on: 20 lut 2023
 *      Author: maswi
 */

#ifndef INC_MODBUS_H_
#define INC_MODBUS_H_
#include "stdio.h"
uint16_t mmodbus_crc16(const uint8_t *nData, uint16_t wLength);
#endif /* INC_MODBUS_H_ */
