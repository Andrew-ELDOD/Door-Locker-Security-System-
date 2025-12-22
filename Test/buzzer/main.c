#include "tm4c123gh6pm.h"
#include "SYSTICK.h"
#include "dio.h"
#define BUZZER_PORT    PORTA
#define BUZZER_PIN     PIN5
void Buzzer_Init(void)
{
    DIO_Init(BUZZER_PORT, BUZZER_PIN, OUTPUT);
    DIO_WritePin(BUZZER_PORT, BUZZER_PIN, LOW);
}
void Buzzer_On(void)  { DIO_WritePin(BUZZER_PORT, BUZZER_PIN, HIGH);}
void Buzzer_Off(void) { DIO_WritePin(BUZZER_PORT, BUZZER_PIN, LOW); }

void Buzzer_pip(void)
{
    DIO_WritePin(BUZZER_PORT, BUZZER_PIN, LOW);
    delayms(1000);
    DIO_WritePin(BUZZER_PORT, BUZZER_PIN, HIGH);
    delayms(1000);
}

int main()
{
  SYSTICK_init(16000);
  Buzzer_Init();
  //RGB_Init();
  while(1)
  {
        Buzzer_pip();
  }
}
