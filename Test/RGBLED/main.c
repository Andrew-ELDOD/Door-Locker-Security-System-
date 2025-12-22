#include "tm4c123gh6pm.h"
#include "dio.h"
#include "SYSTICK.h"
#include <stdio.h>

/* ---------- RGB LED ---------- */
#define RGB_PORT  PORTF
#define LED_RED   PIN1
#define LED_BLUE  PIN2
#define LED_GREEN PIN3
void RGB_Init(void)
{
    DIO_Init(RGB_PORT, LED_RED, OUTPUT);
    DIO_Init(RGB_PORT, LED_BLUE, OUTPUT);
    DIO_Init(RGB_PORT, LED_GREEN, OUTPUT);
}
void Door_Locked(void)
{
    DIO_WritePin(RGB_PORT, LED_RED, 0);
    DIO_WritePin(RGB_PORT, LED_GREEN, 1);
    DIO_WritePin(RGB_PORT, LED_BLUE, 1);
}
void Door_Unlocked(void)
{
    DIO_WritePin(RGB_PORT, LED_GREEN, 0);
    DIO_WritePin(RGB_PORT, LED_RED, 0);
    DIO_WritePin(RGB_PORT, LED_BLUE, 0);
}
void BlinkBlue(uint8_t t)
{
    for(uint8_t i=0;i<t;i++)
    {
        DIO_WritePin(RGB_PORT, LED_BLUE, 0);
        delayms(200);
        DIO_WritePin(RGB_PORT, LED_BLUE, 1);
        delayms(200);
    }
}
int main()
{
  SYSTICK_init(16000);
  RGB_Init();
  
  while(1)
  {
    Door_Locked();
    delayms(1000);
    BlinkBlue(5);
    Door_Unlocked();
    delayms(1000);
    
    
  }
}
