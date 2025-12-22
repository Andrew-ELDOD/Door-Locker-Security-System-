#include "lcd_i2c.h"
#include "tm4c123gh6pm.h"
#include "SYSTICK.h"
#include <stdint.h>

/* PCF8574 bit mapping */
#define LCD_RS  0x01
#define LCD_RW  0x02
#define LCD_EN  0x04
#define LCD_BL  0x08

static uint8_t backlight = LCD_BL;

/* ================= I2C LOW LEVEL ================= */

static void I2C0_Init(void)
{
    SYSTICK_init(16000);
    SYSCTL_RCGCI2C_R |= 0x01;
    SYSCTL_RCGCGPIO_R |= 0x02;

    while(!(SYSCTL_PRGPIO_R & 0x02));

    GPIO_PORTB_AFSEL_R |= 0x0C;
    GPIO_PORTB_ODR_R   |= 0x08;
    GPIO_PORTB_DEN_R   |= 0x0C;
    GPIO_PORTB_PCTL_R  |= 0x00002200;

    I2C0_MCR_R = 0x10;
    I2C0_MTPR_R = 7;   // 100kHz @ 16MHz
}

static void I2C0_WriteByte(uint8_t data)
{
    I2C0_MSA_R = (LCD_I2C_ADDR << 1);
    I2C0_MDR_R = data;
    I2C0_MCS_R = 0x07;

    while(I2C0_MCS_R & 1);
}

/* ================= LCD LOW LEVEL ================= */

static void LCD_Write4Bits(uint8_t nibble, uint8_t mode)
{
    uint8_t data = backlight;

    if(mode) data |= LCD_RS;

    data |= (nibble & 0x0F) << 4;  // D4–D7 on P4–P7

    I2C0_WriteByte(data | LCD_EN);
    delayms(1);
    I2C0_WriteByte(data & ~LCD_EN);
    delayms(1);
}


static void LCD_Send(uint8_t value, uint8_t mode)
{
    LCD_Write4Bits(value >> 4, mode);
    LCD_Write4Bits(value & 0x0F, mode);
}

/* ================= PUBLIC FUNCTIONS ================= */

void LCD_I2C_Init(void)
{
    I2C0_Init();
    delayms(50);

    LCD_Write4Bits(0x03, 0);
    delayms(5);
    LCD_Write4Bits(0x03, 0);
    delayms(1);
    LCD_Write4Bits(0x03, 0);
    delayms(1);
    LCD_Write4Bits(0x02, 0);

    LCD_I2C_SendCommand(LCD_FUNCTION_SET);
    LCD_I2C_SendCommand(LCD_DISPLAY_ON);
    LCD_I2C_SendCommand(LCD_ENTRY_MODE);
    LCD_I2C_Clear();
}

void LCD_I2C_SendCommand(uint8_t cmd)
{
    LCD_Send(cmd, 0);
    delayms(2);
}

void LCD_I2C_Clear(void)
{
    LCD_I2C_SendCommand(LCD_CLEAR);
    delayms(2);
}

void LCD_I2C_SetCursor(uint8_t row, uint8_t col)
{
    uint8_t addr = (row == 0) ? LCD_LINE1 : LCD_LINE2;
    LCD_I2C_SendCommand(addr + col);
}

void LCD_I2C_WriteChar(char c)
{
    LCD_Send(c, 1);
}

void LCD_I2C_WriteString(const char *str)
{
    while(*str)
    {
        LCD_I2C_WriteChar(*str++);
    }
}
