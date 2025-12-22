#include "tm4c123gh6pm.h"
#include "uart.h"

#define SYSTEM_CLOCK    16000000UL
#define BAUD_RATE       115200UL

void UART1_Init(void)
{
    volatile uint32_t delay;
    /* 1. Enable clock for UART1 */
    SYSCTL_RCGCUART_R |= 0x02;  // UART1
    delay = SYSCTL_RCGCUART_R;
    /* 2. Enable clock for GPIOB */
    SYSCTL_RCGCGPIO_R |= 0x02;  // Port B
    delay = SYSCTL_RCGCGPIO_R;
    UART1_CTL_R &= ~UART_CTL_UARTEN;
    UART1_IBRD_R = 8;
    UART1_FBRD_R = 44;

    /* 5. Line Control: 8-bit, no parity, 1 stop bit, FIFO */
    UART1_LCRH_R = UART_LCRH_WLEN_8 | UART_LCRH_FEN;

    /* 6. Enable UART1, TX, RX */
    UART1_CTL_R = UART_CTL_UARTEN | UART_CTL_TXE | UART_CTL_RXE;

    /* 7. Configure PB0 (RX) and PB1 (TX) */
    GPIO_PORTB_AFSEL_R |= 0x03;               // PB0 = RX, PB1 = TX
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFFFFFF00) | 0x00000011; // UART function
    GPIO_PORTB_DEN_R |= 0x03;                 // Digital enable
    GPIO_PORTB_AMSEL_R &= ~0x03;              // Disable analog
}

void UART1_SendChar(char data)
{
    while(UART1_FR_R & UART_FR_TXFF);
    UART1_DR_R = data;
}

char UART1_ReceiveChar(void)
{
    while(UART1_FR_R & UART_FR_RXFE);
    return (char)(UART1_DR_R & 0xFF);
}

void UART1_SendString(const char *str)
{
    while(*str)
    {
        UART1_SendChar(*str);
        str++;
    }
}
uint8_t UART1_IsDataAvailable(void)
{
    return ((UART1_FR_R & UART_FR_RXFE) == 0) ? 1 : 0;
}
