#include "potentiometer.h"
#include "adc.h"

void POT_Init(void)
{
    ADC_Init(POT_ADC_CHANNEL);
}

uint16_t POT_ReadRaw(void)
{
    return ADC_Read();
}

uint32_t POT_ReadMillivolts(void)
{
    uint16_t rawValue = ADC_Read();
    return ADC_ToMillivolts(rawValue);
}

uint8_t POT_ReadPercentage(void)
{
    uint16_t rawValue = ADC_Read();
    return (uint8_t)((rawValue * 100UL) / 4095UL);
}
uint32_t POT_ReadMapped(uint32_t min, uint32_t max)
{
    uint16_t rawValue = ADC_Read();
    return min + ((rawValue * (max - min)) / 4095UL);
}
