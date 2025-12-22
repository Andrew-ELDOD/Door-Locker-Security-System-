#include "tm4c123gh6pm.h"
#include "SYSTICK.h"
#include "lcd_i2c.h"
#include "keypad.h"
#include "potentiometer.h"
#include "eeprom.h"
#include <string.h>
#include <stdio.h>

#define PASSWORD_LEN 5
#define EEPROM_BLOCK 0
#define EEPROM_PASS_OFFSET 0
#define EEPROM_TIMEOUT_OFFSET 4

char PASSWORD[PASSWORD_LEN+1];
char entered[PASSWORD_LEN+1];
char stored_password[PASSWORD_LEN+1];
uint8_t index = 0;
uint8_t tries = 3;
uint32_t autoLockTime = 10;

// ---- RGB LEDs ----
void RGB_Init(void){ SYSCTL_RCGCGPIO_R|=0x20; GPIO_PORTF_DIR_R|=0x0E; GPIO_PORTF_DEN_R|=0x0E; }
void Door_Lock(void){GPIO_PORTF_DATA_R = 0x02;}  // Red
void Door_Unlock(void){GPIO_PORTF_DATA_R = 0x08;} // Green
void StatusLED_On(void){GPIO_PORTF_DATA_R |= 0x04;}
void StatusLED_Off(void){GPIO_PORTF_DATA_R &= ~0x04;}
void StatusLED_Blink(uint8_t times){for(uint8_t i=0;i<times;i++){StatusLED_On(); delayms(200); StatusLED_Off(); delayms(200);}}

// ---- EEPROM Functions ----
uint8_t StorePasswordEEPROM(void)
{
    uint8_t buffer[8]={0};
    for(uint8_t i=0;i<PASSWORD_LEN;i++) buffer[i]=PASSWORD[i];
    return EEPROM_WriteBuffer(EEPROM_BLOCK, EEPROM_PASS_OFFSET, buffer,8);
}

uint8_t RetrievePasswordEEPROM(void)
{
    uint8_t buffer[8]={0};
    if(EEPROM_ReadBuffer(EEPROM_BLOCK, EEPROM_PASS_OFFSET, buffer,8)!=EEPROM_SUCCESS) return 0;

    // Check if password area empty
    uint8_t empty=1;
    for(uint8_t i=0;i<PASSWORD_LEN;i++){if(buffer[i]!=0xFF){empty=0; break;}}
    if(empty) return 0;

    for(uint8_t i=0;i<PASSWORD_LEN;i++) stored_password[i]=buffer[i];
    stored_password[PASSWORD_LEN]='\0';
    return 1;
}

uint8_t StoreTimeoutEEPROM(void)
{
    uint8_t buffer[4]={autoLockTime,0,0,0};
    return EEPROM_WriteBuffer(EEPROM_BLOCK, EEPROM_TIMEOUT_OFFSET, buffer,4);
}

uint8_t RetrieveTimeoutEEPROM(void)
{
    uint8_t buffer[4]={0};
    if(EEPROM_ReadBuffer(EEPROM_BLOCK, EEPROM_TIMEOUT_OFFSET, buffer,4)!=EEPROM_SUCCESS) return 0;
    autoLockTime=buffer[0];
    if(autoLockTime<5||autoLockTime>30) autoLockTime=10;
    return 1;
}

// ---- Reset Password ----
void ResetPasswordEEPROM(void)
{
    uint8_t empty[8]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    EEPROM_WriteBuffer(EEPROM_BLOCK, EEPROM_PASS_OFFSET, empty,8);
}

// ---- UI Functions ----
void Set_Pass(void)
{
    LCD_I2C_Clear();
    LCD_I2C_WriteString("Set Password");
    index=0;
    char key;
    while(index<PASSWORD_LEN)
    {
        key=Keypad_Read();
        if(key){PASSWORD[index++]=key; LCD_I2C_WriteChar('*'); delayms(300);}
    }
    PASSWORD[index]='\0';
    StorePasswordEEPROM();
}

void Set_Timeout_UI(void)
{
    char key;
    uint32_t time;
    LCD_I2C_Clear();
    LCD_I2C_WriteString("Adjust Timeout");
    while(1)
    {
        time=POT_ReadMapped(5,30);
        LCD_I2C_SetCursor(1,0);
        char buf[16]; sprintf(buf,"Time:%02d sec",time); LCD_I2C_WriteString(buf);
        key=Keypad_Read();
        if(key=='#'){autoLockTime=time; StoreTimeoutEEPROM(); LCD_I2C_Clear(); LCD_I2C_WriteString("Saved!"); delayms(1000); return;}
    }
}

// ---- Menu after access granted ----
void MainMenu(void)
{
    char key;
    while(1)
    {
        LCD_I2C_Clear();
        LCD_I2C_WriteString("1:Open 2:Pass");
        LCD_I2C_SetCursor(1,0);
        LCD_I2C_WriteString("3:Timeout 4:Reset");
        key=0;
        while(!key) key=Keypad_Read();
        if(key=='1') { Door_Unlock(); delayms(autoLockTime*1000); Door_Lock(); }
        else if(key=='2') { Set_Pass(); }
        else if(key=='3') { Set_Timeout_UI(); }
        else if(key=='4') { ResetPasswordEEPROM(); LCD_I2C_Clear(); LCD_I2C_WriteString("Pass Reset"); delayms(1000); Set_Pass(); }
    }
}

// ---- Main ----
int main(void)
{
    char key;
    POT_Init();
    RGB_Init();
    LCD_I2C_Init();
    Keypad_Init();
    EEPROM_Init();

    if(!RetrievePasswordEEPROM()){Set_Pass();}
    if(!RetrieveTimeoutEEPROM()) autoLockTime=10;

    while(1)
    {
        LCD_I2C_Clear();
        LCD_I2C_WriteString("Enter Password");
        index=0;
        while(index<PASSWORD_LEN)
        {
            key=Keypad_Read();
            if(key){entered[index++]=key; LCD_I2C_WriteChar('*'); delayms(300);}
        }
        entered[index]='\0';
        RetrievePasswordEEPROM();

        if(strcmp(entered,stored_password)==0)
        {
            LCD_I2C_Clear();
            LCD_I2C_WriteString("ACCESS GRANTED");
            Door_Unlock();
            delayms(500);
            Door_Lock();
            tries=3;
            MainMenu(); // Enter menu after access
        }
        else
        {
            tries--;
            LCD_I2C_Clear();
            LCD_I2C_WriteString("WRONG PASSWORD");
            LCD_I2C_SetCursor(1,0);
            char buf[16]; sprintf(buf,"Tries Left:%d",tries); LCD_I2C_WriteString(buf);
            delayms(1500);
            if(tries==0)
            {
                LCD_I2C_Clear();
                LCD_I2C_WriteString("ACCESS DENIED");
                GPIO_PORTF_DATA_R=0x02;
                while(1);
            }
        }
    }
}
