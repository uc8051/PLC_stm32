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
        OUT(1, ON);
    } else {
        OUT(1, OFF);
    }

    if (IN(2)) {
        OUT(2, ON);
        OUT(3, ON);
        OUT(4, ON);
        OUT(5, ON);
        OUT(6, ON);
    } else {
        OUT(2, OFF);
        OUT(3, OFF);
        OUT(4, OFF);
        OUT(5, OFF);
        OUT(6, OFF);

    }


}
