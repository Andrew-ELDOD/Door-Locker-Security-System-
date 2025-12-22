#ifndef KEYPAD_H
#define KEYPAD_H

#include "tm4c123gh6pm.h"
#include "SYSTICK.h"

/* Keypad initialization */
void Keypad_Init(void);

/* Read pressed key
   Returns:
   0  -> no key
   '1'..'9','0','A'..'D','*','#'
*/
char Keypad_Read(void);

#endif
