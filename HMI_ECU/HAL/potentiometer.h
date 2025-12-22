#ifndef POTENTIOMETER_H_
#define POTENTIOMETER_H_

#include <stdint.h>

#define POT_PIN             3       /* PE3 */
#define POT_ADC_CHANNEL     0       /* AIN0 */

void POT_Init(void);

uint16_t POT_ReadRaw(void);

uint32_t POT_ReadMillivolts(void);

uint8_t POT_ReadPercentage(void);

uint32_t POT_ReadMapped(uint32_t min, uint32_t max);

#endif 
