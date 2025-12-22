#include "keypad.h"
#include "tm4c123gh6pm.h"
#include "SYSTICK.h"
#include <stdint.h>

/* ================= KEYPAD INIT ================= */
void Keypad_Init(void)
{
    SYSTICK_init(16000);

    /* Enable Port A & C */
    SYSCTL_RCGCGPIO_R |= (1 << 0) | (1 << 2);
    while((SYSCTL_PRGPIO_R & ((1 << 0) | (1 << 2))) == 0);

    /* Rows: PA2–PA5 OUTPUT */
    GPIO_PORTA_DIR_R |= 0x3C;
    GPIO_PORTA_DEN_R |= 0x3C;
    GPIO_PORTA_DATA_R |= 0x3C;   // all rows HIGH

    /* Columns: PC4–PC7 INPUT with pull-up */
    GPIO_PORTC_DIR_R &= ~0xF0;
    GPIO_PORTC_DEN_R |= 0xF0;
    GPIO_PORTC_PUR_R |= 0xF0;
}

/* ================= KEYPAD READ ================= */
char Keypad_Read(void)
{
    const char keyMap[4][4] = {
        {'1','2','3','A'},
        {'4','5','6','B'},
        {'7','8','9','C'},
        {'*','0','#','D'}
    };

    for(int row = 0; row < 4; row++)
    {
        /* Drive one row LOW (PA2–PA5 only) */
       GPIO_PORTA_DATA_R = ~(1 << (row + 2));

        delayms(2);

        uint8_t col = (GPIO_PORTC_DATA_R >> 4) & 0x0F;

        if(col != 0x0F)
        {
            delayms(20);  // debounce

            if(!(col & 0x01)) return keyMap[row][0];
            if(!(col & 0x02)) return keyMap[row][1];
            if(!(col & 0x04)) return keyMap[row][2];
            if(!(col & 0x08)) return keyMap[row][3];
        }
    }
    return 0;
}
