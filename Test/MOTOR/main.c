#include "tm4c123gh6pm.h"
#include "motor.h"
#include "SYSTICK.h"
#include "dio.h"


int main(void)
{
  SYSTICK_init(16000);
  Motor_Init();

    while(1)
    {
      Motor_RotateCW(); 
      delayms(2000);

       Motor_Stop(); 
       delayms(2000);

        Motor_RotateCCW(); 
        delayms(2000);
    }
}
