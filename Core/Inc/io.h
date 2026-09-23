/*
 * io.h
 *
 *  Created on: Sep 23, 2026
 *      Author: maswi
 */

#ifndef INC_IO_H_
#define INC_IO_H_
#include <stdint.h>
#include <stdbool.h>

/* Stan wyjścia */
#define SET     true
#define RESET   false

/* Dozwolone numery:
   0..7
   10..17
   20..27
*/
#define IO_VALID(n) \
    (((n) >= 0 && (n) <= 7) || \
     ((n) >= 10 && (n) <= 17) || \
     ((n) >= 20 && (n) <= 27))

/* Zamiana numeru PLC na bit 0..23 */
#define IO_BIT(n) \
    (((n) / 10) * 8 + ((n) % 10))

/* Sprawdzenie numeru podczas kompilacji */
#define IO_CHECK(n) \
    ((void)sizeof(char[(IO_VALID(n)) ? 1 : -1]))

/* Funkcje wewnętrzne */
bool IO_InputGet(uint8_t bit);
bool IO_OutputGet(uint8_t bit);
void IO_OutputSet(uint8_t bit, bool state);

void IO_ReadInputs(void);
void IO_WriteOutputs(void);

/* ===== API używane w programie PLC ===== */

#define IN(n) \
    (IO_CHECK(n), IO_InputGet(IO_BIT(n)))

#define IS_OUT(n) \
    (IO_CHECK(n), IO_OutputGet(IO_BIT(n)))

#define OUT(n, state) \
    do { \
        IO_CHECK(n); \
        IO_OutputSet(IO_BIT(n), (state)); \
    } while (0)



#endif /* INC_IO_H_ */
