#ifndef BUZZER_H_
#define BUZZER_H_

#include <stdint.h>

/* Initialize buzzer GPIO */
void Buzzer_Init(void);

/* Beep t times */
void Buzzer_Beep(uint8_t times);

/* Turn buzzer ON */
void Buzzer_On(void);

/* Turn buzzer OFF */
void Buzzer_Off(void);

#endif
