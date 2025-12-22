#include "tm4c123gh6pm.h"
#include "uart.h"

void RGB_Init(void) {
    SYSCTL_RCGCGPIO_R |= 0x20;   // Port F
    GPIO_PORTF_DIR_R |= 0x0E;
    GPIO_PORTF_DEN_R |= 0x0E;
}

int main(void) {
    char key;

    RGB_Init();
    UART1_Init();

    while(1) {
        if(UART1_IsDataAvailable()) {
            key = UART1_ReceiveChar();

            switch(key) {
                case '7': GPIO_PORTF_DATA_R = 0x02; break; // Red
                case '8': GPIO_PORTF_DATA_R = 0x04; break; // Blue
                case '9': GPIO_PORTF_DATA_R = 0x08; break; // Green
                case 'C': GPIO_PORTF_DATA_R = 0x0E; break; // White
                case '#': GPIO_PORTF_DATA_R = 0x00; break; // OFF
                default:  break;
            }
        }
    }
}
