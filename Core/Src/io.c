/*
 * io.c
 *
 *  Created on: Sep 23, 2026
 *      Author: maswi
 */

#include "io.h"
#include "main.h"

static uint32_t inputs = 0;
static uint32_t outputs = 0;


bool IO_InputGet(uint8_t bit)
{
    return (inputs & (1UL << bit)) != 0;
}


bool IO_OutputGet(uint8_t bit)
{
    return (outputs & (1UL << bit)) != 0;
}


void IO_OutputSet(uint8_t bit, bool state)
{
    if (state)
        outputs |= (1UL << bit);
    else
        outputs &= ~(1UL << bit);
}

void IO_ReadInputs(void)
{
    uint32_t temp = 0;

    if (HAL_GPIO_ReadPin(in00_GPIO_Port, in00_Pin))
        temp |= (1UL << 0);

    if (HAL_GPIO_ReadPin(in01_GPIO_Port, in01_Pin))
        temp |= (1UL << 1);

    if (HAL_GPIO_ReadPin(in02_GPIO_Port, in02_Pin))
        temp |= (1UL << 2);

    /* ... */

    if (HAL_GPIO_ReadPin(in10_GPIO_Port, in10_Pin))
        temp |= (1UL << 8);

    /* ... */



    inputs = temp;
}


void IO_WriteOutputs(void)
{
    HAL_GPIO_WritePin(
        out00_GPIO_Port,
        out00_Pin,
        (outputs & (1UL << 0)) ? GPIO_PIN_SET : GPIO_PIN_RESET
    );

    HAL_GPIO_WritePin(
        out01_GPIO_Port,
        out01_Pin,
        (outputs & (1UL << 1)) ? GPIO_PIN_SET : GPIO_PIN_RESET
    );

    /* itd... */
}

