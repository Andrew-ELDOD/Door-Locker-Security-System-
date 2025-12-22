#include "buzzer.h"
#include "dio.h"
#include "SYSTICK.h"

/* -------- Configuration -------- */
#define BUZZER_PORT PORTA
#define BUZZER_PIN  PIN5

void Buzzer_Init(void)
{
    DIO_Init(BUZZER_PORT, BUZZER_PIN, OUTPUT);
    DIO_WritePin(BUZZER_PORT, BUZZER_PIN, 0);
}

void Buzzer_On(void)
{
    DIO_WritePin(BUZZER_PORT, BUZZER_PIN, 1);
}

void Buzzer_Off(void)
{
    DIO_WritePin(BUZZER_PORT, BUZZER_PIN, 0);
}

void Buzzer_Beep(uint8_t times)
{
    for(uint8_t i=0;i<times;i++)
    {
        Buzzer_On();
        delayms(300);
        Buzzer_Off();
        delayms(300);
    }
}
