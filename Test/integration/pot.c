#include "pot.h"
#include "tm4c123gh6pm.h"
#include <stdint.h>

/* PE3 ? ADC0 SS3 (AIN0) */

void POT1_Init(void)
{
    SYSCTL_RCGCGPIO_R |= 0x10;   // Port E
    SYSCTL_RCGCADC_R  |= 0x01;   // ADC0
    while(!(SYSCTL_PRGPIO_R & 0x10));

    GPIO_PORTE_DIR_R &= ~0x08;
    GPIO_PORTE_AFSEL_R |= 0x08;
    GPIO_PORTE_DEN_R &= ~0x08;
    GPIO_PORTE_AMSEL_R |= 0x08;

    ADC0_ACTSS_R &= ~0x08;
    ADC0_EMUX_R &= ~0xF000;
    ADC0_SSMUX3_R = 0;
    ADC0_SSCTL3_R = 0x06;
    ADC0_ACTSS_R |= 0x08;
}

uint16_t POT_Read(void)
{
    ADC0_PSSI_R = 0x08;
    while(!(ADC0_RIS_R & 0x08));
    uint16_t value = ADC0_SSFIFO3_R & 0xFFF;
    ADC0_ISC_R = 0x08;
    return value;
}

uint32_t POT_GetTimeout(void)
{
    uint16_t adc = POT_Read();
    return 5 + ((adc * 25) / 4095);   // 5–30 sec
}
