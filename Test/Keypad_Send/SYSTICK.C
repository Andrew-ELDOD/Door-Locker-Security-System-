#include "tm4c123gh6pm.h"

void SYSTICK_init(int reload)
{
  NVIC_ST_CTRL_R = 0X0;
  NVIC_ST_RELOAD_R = reload -1; // 1ms
  NVIC_ST_CURRENT_R= 0;
  NVIC_ST_CTRL_R = 0X5;
  }
  
  void delayms(int time)
{
         for (int i=0; i<time;i++)
         while((NVIC_ST_CTRL_R &(1<<16)) == 0 );
}