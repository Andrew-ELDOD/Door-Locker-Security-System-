#ifndef UART_H
#define UART_H

#include <stdint.h>

/* UART1 functions (PB0 = RX, PB1 = TX) */
void UART1_Init(void);
void UART1_SendChar(char data);
char UART1_ReceiveChar(void);
void UART1_SendString(const char *str);
uint8_t UART1_IsDataAvailable(void);

#endif
  