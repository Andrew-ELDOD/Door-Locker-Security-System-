#include "tm4c123gh6pm.h"
#include "uart.h"
#include "keypad.h"

void RGB_Init(void) {
    SYSCTL_RCGCGPIO_R |= 0x20;   // Port F
    GPIO_PORTF_DIR_R |= 0x0E;
    GPIO_PORTF_DEN_R |= 0x0E;
}

int main(void) {
    char key, lastKey = 0;

    RGB_Init();       // Optional if you want local LED feedback
    Keypad_Init();
    UART1_Init();

    while(1) {
        key = Keypad_Read();
 
        // Simple debounce: only send new key
        if(key != 0 && key != lastKey) {
            UART1_SendChar(key);
            lastKey = key;
        } else if(key == 0) {
            lastKey = 0;
        }
        
        for(volatile int i=0; i<20000; i++); // small delay for debounce
    }
}

