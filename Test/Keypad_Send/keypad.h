#ifndef KEYPAD_H
#define KEYPAD_H

#include "tm4c123gh6pm.h"
#include "SYSTICK.h"

/* Keypad initialization */
void Keypad_Init(void);
char Keypad_Read(void);

#endif
