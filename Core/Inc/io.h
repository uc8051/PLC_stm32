#ifndef INC_IO_H_
#define INC_IO_H_

#include <stdint.h>
#include <stdbool.h>
#include "board.h"

#define ON   true
#define OFF  false

#if PCB_VERSION == PCB_14

#define IN_VALID(n) \
    (((n) >= 0 && (n) <= 7) || ((n) >= 10 && (n) <= 15))

#define OUT_VALID(n) \
    (((n) >= 0 && (n) <= 7) || ((n) >= 10 && (n) <= 11))

#elif PCB_VERSION == PCB_24

#define IN_VALID(n) \
    (((n) >= 0 && (n) <= 7) || \
     ((n) >= 10 && (n) <= 17) || \
     ((n) >= 20 && (n) <= 27))

#define OUT_VALID(n) \
    (((n) >= 0 && (n) <= 7) || \
     ((n) >= 10 && (n) <= 17) || \
     ((n) >= 20 && (n) <= 27))

#else
#error "Nieznana wersja PCB"
#endif

/* 00..07 -> bity 0..7; 10..17 -> 8..15; 20..27 -> 16..23 */
#define IO_BIT(n) ((((n) / 10) * 8) + ((n) % 10))

/* Bledny staly numer IN/OUT powoduje blad kompilacji. */
#define IN_CHECK(n)  ((void)sizeof(char[(IN_VALID(n)) ? 1 : -1]))
#define OUT_CHECK(n) ((void)sizeof(char[(OUT_VALID(n)) ? 1 : -1]))

bool IO_InputGet(uint8_t bit);
bool IO_OutputGet(uint8_t bit);
void IO_OutputSet(uint8_t bit, bool state);
uint8_t IO_GetInputGroup(uint8_t group);
uint8_t IO_GetOutputGroup(uint8_t group);
void IO_SetOutputGroup(uint8_t group, uint8_t value);


void IO_ReadInputs(void);
void IO_WriteOutputs(void);

#define IN(n) \
    (IN_CHECK(n), IO_InputGet(IO_BIT(n)))

#define IS_OUT(n) \
    (OUT_CHECK(n), IO_OutputGet(IO_BIT(n)))

#define OUT(n, state) \
    do { \
        OUT_CHECK(n); \
        IO_OutputSet(IO_BIT(n), (state)); \
    } while (0)

#endif /* INC_IO_H_ */
