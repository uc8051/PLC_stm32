/*
 * plc.c
 *
 *  Created on: Sep 23, 2026
 *      Author: maswi
 */

#include "plc.h"
#include "io.h"


void PLC_Init(void)
{
    /* rzeczy wykonywane raz po uruchomieniu */
}


void PLC_Run(void)
{
    /*
     * TUTAJ PROGRAM STEROWNIKA
     */

    if (IN(1)) {
        OUT(3, ON);
    } else {
        OUT(3, OFF);
    }



}
