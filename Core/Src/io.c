#include "io.h"
#include "main.h"

 uint32_t inputs  = 0;
 uint32_t outputs = 0;

bool IO_InputGet(uint8_t bit)
{
    return (inputs & (1UL << bit)) != 0U;
}

bool IO_OutputGet(uint8_t bit)
{
    return (outputs & (1UL << bit)) != 0U;
}

void IO_OutputSet(uint8_t bit, bool state)
{
    if (state) {
        outputs |= (1UL << bit);
    } else {
        outputs &= ~(1UL << bit);
    }
}

void IO_ReadInputs(void)
{
    uint32_t temp = 0;

    if (HAL_GPIO_ReadPin(in00_GPIO_Port, in00_Pin) == GPIO_PIN_RESET) temp |= (1UL << 0);
    if (HAL_GPIO_ReadPin(in01_GPIO_Port, in01_Pin) == GPIO_PIN_RESET) temp |= (1UL << 1);
    if (HAL_GPIO_ReadPin(in02_GPIO_Port, in02_Pin) == GPIO_PIN_RESET) temp |= (1UL << 2);
    if (HAL_GPIO_ReadPin(in03_GPIO_Port, in03_Pin) == GPIO_PIN_RESET) temp |= (1UL << 3);
    if (HAL_GPIO_ReadPin(in04_GPIO_Port, in04_Pin) == GPIO_PIN_RESET) temp |= (1UL << 4);
    if (HAL_GPIO_ReadPin(in05_GPIO_Port, in05_Pin) == GPIO_PIN_RESET) temp |= (1UL << 5);
    if (HAL_GPIO_ReadPin(in06_GPIO_Port, in06_Pin) == GPIO_PIN_RESET) temp |= (1UL << 6);
    if (HAL_GPIO_ReadPin(in07_GPIO_Port, in07_Pin) == GPIO_PIN_RESET) temp |= (1UL << 7);

    if (HAL_GPIO_ReadPin(in10_GPIO_Port, in10_Pin) == GPIO_PIN_RESET) temp |= (1UL << 8);
    if (HAL_GPIO_ReadPin(in11_GPIO_Port, in11_Pin) == GPIO_PIN_RESET) temp |= (1UL << 9);
    if (HAL_GPIO_ReadPin(in12_GPIO_Port, in12_Pin) == GPIO_PIN_RESET) temp |= (1UL << 10);
    if (HAL_GPIO_ReadPin(in13_GPIO_Port, in13_Pin) == GPIO_PIN_RESET) temp |= (1UL << 11);
    if (HAL_GPIO_ReadPin(in14_GPIO_Port, in14_Pin) == GPIO_PIN_RESET) temp |= (1UL << 12);
    if (HAL_GPIO_ReadPin(in15_GPIO_Port, in15_Pin) == GPIO_PIN_RESET) temp |= (1UL << 13);

#if PCB_VERSION == PCB_24

    if (HAL_GPIO_ReadPin(in16_GPIO_Port, in16_Pin) == GPIO_PIN_RESET) temp |= (1UL << 14);
    if (HAL_GPIO_ReadPin(in17_GPIO_Port, in17_Pin) == GPIO_PIN_RESET) temp |= (1UL << 15);

    /*if (HAL_GPIO_ReadPin(in20_GPIO_Port, in20_Pin) == GPIO_PIN_RESET) temp |= (1UL << 16);
    if (HAL_GPIO_ReadPin(in21_GPIO_Port, in21_Pin) == GPIO_PIN_RESET) temp |= (1UL << 17);
    if (HAL_GPIO_ReadPin(in22_GPIO_Port, in22_Pin) == GPIO_PIN_RESET) temp |= (1UL << 18);
    if (HAL_GPIO_ReadPin(in23_GPIO_Port, in23_Pin) == GPIO_PIN_RESET) temp |= (1UL << 19);
    if (HAL_GPIO_ReadPin(in24_GPIO_Port, in24_Pin) == GPIO_PIN_RESET) temp |= (1UL << 20);
    if (HAL_GPIO_ReadPin(in25_GPIO_Port, in25_Pin) == GPIO_PIN_RESET) temp |= (1UL << 21);
    if (HAL_GPIO_ReadPin(in26_GPIO_Port, in26_Pin) == GPIO_PIN_RESET) temp |= (1UL << 22);
    if (HAL_GPIO_ReadPin(in27_GPIO_Port, in27_Pin) == GPIO_PIN_RESET) temp |= (1UL << 23);*/

#endif

    inputs = temp;
}

void IO_WriteOutputs(void)
{
    /* OUT 00..07 */
    HAL_GPIO_WritePin(out00_GPIO_Port, out00_Pin, (outputs & (1UL << 0)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(out01_GPIO_Port, out01_Pin, (outputs & (1UL << 1)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(out02_GPIO_Port, out02_Pin, (outputs & (1UL << 2)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(out03_GPIO_Port, out03_Pin, (outputs & (1UL << 3)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(out04_GPIO_Port, out04_Pin, (outputs & (1UL << 4)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(out05_GPIO_Port, out05_Pin, (outputs & (1UL << 5)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(out06_GPIO_Port, out06_Pin, (outputs & (1UL << 6)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(out07_GPIO_Port, out07_Pin, (outputs & (1UL << 7)) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    /* OUT 10..11 */
    HAL_GPIO_WritePin(out10_GPIO_Port, out10_Pin, (outputs & (1UL << 8)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(out11_GPIO_Port, out11_Pin, (outputs & (1UL << 9)) ? GPIO_PIN_SET : GPIO_PIN_RESET);

#if PCB_VERSION == PCB_24
    /* Te nazwy GPIO dopisz/ustaw w CubeMX dla wersji PCB_24. */
    HAL_GPIO_WritePin(out12_GPIO_Port, out12_Pin, (outputs & (1UL << 10)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(out13_GPIO_Port, out13_Pin, (outputs & (1UL << 11)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(out14_GPIO_Port, out14_Pin, (outputs & (1UL << 12)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(out15_GPIO_Port, out15_Pin, (outputs & (1UL << 13)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(out16_GPIO_Port, out16_Pin, (outputs & (1UL << 14)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(out17_GPIO_Port, out17_Pin, (outputs & (1UL << 15)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    /*HAL_GPIO_WritePin(out20_GPIO_Port, out20_Pin, (outputs & (1UL << 16)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(out21_GPIO_Port, out21_Pin, (outputs & (1UL << 17)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(out22_GPIO_Port, out22_Pin, (outputs & (1UL << 18)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(out23_GPIO_Port, out23_Pin, (outputs & (1UL << 19)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(out24_GPIO_Port, out24_Pin, (outputs & (1UL << 20)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(out25_GPIO_Port, out25_Pin, (outputs & (1UL << 21)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(out26_GPIO_Port, out26_Pin, (outputs & (1UL << 22)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(out27_GPIO_Port, out27_Pin, (outputs & (1UL << 23)) ? GPIO_PIN_SET : GPIO_PIN_RESET);*/
#endif
}

uint8_t IO_GetInputGroup(uint8_t group)
{
    switch (group)
    {
        case 0:
            return (uint8_t)(inputs & 0xFF);

        case 1:
            return (uint8_t)((inputs >> 8) & 0xFF);

        case 2:
            return (uint8_t)((inputs >> 16) & 0xFF);

        default:
            return 0;
    }
}


uint8_t IO_GetOutputGroup(uint8_t group)
{
    switch (group)
    {
        case 0:
            return (uint8_t)(outputs & 0xFF);

        case 1:
            return (uint8_t)((outputs >> 8) & 0xFF);

        case 2:
            return (uint8_t)((outputs >> 16) & 0xFF);

        default:
            return 0;
    }
}


void IO_SetOutputGroup(uint8_t group, uint8_t value)
{
    switch (group)
    {
        case 0:
            outputs &= ~0x000000FFUL;
            outputs |= (uint32_t)value;
            break;

        case 1:
            outputs &= ~0x0000FF00UL;
            outputs |= ((uint32_t)value << 8);
            break;

        case 2:
            outputs &= ~0x00FF0000UL;
            outputs |= ((uint32_t)value << 16);
            break;
    }
}
