#ifndef POT_H
#define POT_H

#include <stdint.h>

void POT1_Init(void);
uint16_t POT_Read(void);
uint32_t POT_GetTimeout(void);   // returns 5–30 seconds

#endif
