#ifndef LCD_I2C_H
#define LCD_I2C_H

#include <stdint.h>

/* PCF8574 I2C address (change if needed) */
#define LCD_I2C_ADDR  0x27   // 0x27 or 0x3F commonly

/* LCD commands */
#define LCD_CLEAR        0x01
#define LCD_HOME         0x02
#define LCD_ENTRY_MODE   0x06
#define LCD_DISPLAY_ON   0x0C
#define LCD_DISPLAY_OFF  0x08
#define LCD_FUNCTION_SET 0x28
#define LCD_LINE1        0x80
#define LCD_LINE2        0xC0

void LCD_I2C_Init(void);
void LCD_I2C_Clear(void);
void LCD_I2C_SetCursor(uint8_t row, uint8_t col);
void LCD_I2C_WriteChar(char c);
void LCD_I2C_WriteString(const char *str);
void LCD_I2C_SendCommand(uint8_t cmd);

#endif
