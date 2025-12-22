#include "tm4c123gh6pm.h"
#include "lcd_i2c.h"
#include "keypad.h"
#include "potentiometer.h"
#include "uart.h"
#include "SYSTICK.h"
#include "dio.h"
#include <string.h>
#include <stdio.h>

#define PASSWORD_LEN 5
#define MAX_TRIES    3

/* ---------- GLOBALS ---------- */
char newPass[PASSWORD_LEN+1];  // last saved password from control board
char entered[PASSWORD_LEN+1];
uint8_t tries = 0;
uint32_t autoLockTime = 10U;

/* ---------- RGB LED ---------- */
#define RGB_PORT  PORTF
#define LED_RED   PIN1
#define LED_BLUE  PIN2
#define LED_GREEN PIN3
void Menu(void);

void RGB_Init(void)
{
    DIO_Init(RGB_PORT, LED_RED, OUTPUT);
    DIO_Init(RGB_PORT, LED_BLUE, OUTPUT);
    DIO_Init(RGB_PORT, LED_GREEN, OUTPUT);
}

void Door_Locked(void)
{
    DIO_WritePin(RGB_PORT, LED_RED, 0);
    DIO_WritePin(RGB_PORT, LED_GREEN, 0);
    DIO_WritePin(RGB_PORT, LED_BLUE, 0);
}

void Door_Unlocked(void)
{
    DIO_WritePin(RGB_PORT, LED_GREEN, 0);
    DIO_WritePin(RGB_PORT, LED_RED, 1);
    DIO_WritePin(RGB_PORT, LED_BLUE, 1);
}

void BlinkRed(uint8_t t)
{
    DIO_WritePin(RGB_PORT, LED_GREEN, 1);
    DIO_WritePin(RGB_PORT, LED_BLUE, 1);
    for(uint8_t i=0;i<t;i++)
    {
        DIO_WritePin(RGB_PORT, LED_RED, 0);
        delayms(200);
        DIO_WritePin(RGB_PORT, LED_RED, 1);
        delayms(200);
    }
}

/* ---------- UART ---------- */
uint8_t SendPassword(char cmd)
{
    UART1_SendChar(cmd);
    for(uint8_t i= 0U;i<PASSWORD_LEN;i++)
        UART1_SendChar(entered[i]);

    return (UART1_ReceiveChar() == '1');
}

void SendNewPassword(void)
{
    UART1_SendChar('C');
    for(uint8_t i= 0U;i<PASSWORD_LEN;i++)
        UART1_SendChar(newPass[i]);
    UART1_ReceiveChar(); // ACK
}

/* ---------- HMI Utilities ---------- */
void WaitKeyRelease(void)
{
    while(Keypad_Read());
}

void LoadPasswordFromControl(void)
{
    for(uint8_t i=0;i<PASSWORD_LEN;i++)
        newPass[i] = UART1_ReceiveChar();
    newPass[sizeof(newPass)/sizeof(newPass[0]) - 1] = '\0';
}

/* ---------- AUTH ---------- */
uint8_t AuthenticateUser(void)
{
    tries = 0;
    while(Keypad_Read());

    while(tries < MAX_TRIES)
    {
        LCD_I2C_Clear();
        LCD_I2C_WriteString("Enter Password:");
        LCD_I2C_SetCursor(1,0);

        uint8_t i=0; char k;
        while(i<PASSWORD_LEN)
        {
            k = Keypad_Read();
            if(k)
            {
                entered[i++] = k;
                LCD_I2C_WriteChar('*');
                delayms(300);
            }
        }
         UART1_SendChar('A');
        for(i = 0; i < PASSWORD_LEN; i++)
            UART1_SendChar(entered[i]);

        char resp = UART1_ReceiveChar(); 
         if(resp == '1')
        {
            SendNewPassword();
            LCD_I2C_Clear();
            LCD_I2C_WriteString("DONE!");
            delayms(1000);
            return 1;
        }
        tries++;
        LCD_I2C_Clear();
        LCD_I2C_WriteString("Wrong!");
        BlinkRed(tries);
        delayms(1000);
    }

    UART1_SendChar('L');
    LCD_I2C_Clear();
    LCD_I2C_WriteString("LOCKED!");
    Door_Locked();
    delayms(15000);
    Menu();
    return 0;
}

/* ---------- SET PASSWORD ---------- */
void SetPassword_FirstTime(void)
{
    while(1)
    {
        // --- Enter new password ---
        LCD_I2C_Clear();
        LCD_I2C_WriteString("Set Password:");
        LCD_I2C_SetCursor(1,0);

        uint8_t i = 0; char k;
        while(i < PASSWORD_LEN)
        {
            k = Keypad_Read();
            if(k)
            {
                newPass[i++] = k;
                LCD_I2C_WriteChar('*');
                delayms(300);
            }
        }
        // --- Confirm password ---
        LCD_I2C_Clear();
        LCD_I2C_WriteString("Confirm:");
        LCD_I2C_SetCursor(1,0);

        i = 0;
        while(i < PASSWORD_LEN)
        {
            k = Keypad_Read();
            if(k)
            {
                entered[i++] = k;
                LCD_I2C_WriteChar('*');
                delayms(300);
            }
        }

        // --- Check match ---
        int cmpResult = memcmp(newPass, entered, PASSWORD_LEN);
        if(cmpResult == 0)
        {
            SendNewPassword();
            LCD_I2C_Clear();
            LCD_I2C_WriteString("Saved!");
            delayms(1000);
            return; // password set successfully
        }
        else
        {
            // Mismatch ? go back to "Set Password"
            LCD_I2C_Clear();
            LCD_I2C_WriteString("Mismatch!");
            delayms(1000);
            // loop continues automatically, no extra code needed
        }
    }
}
/* ---------- OPEN DOOR ---------- */
void OpenDoor(void)
{
    if(!AuthenticateUser()) return;
     SendPassword('O'); 
    LCD_I2C_Clear();
    LCD_I2C_WriteString("ACCESS GRANTED");
    Door_Unlocked();
    delayms(autoLockTime * 1000);
    Door_Locked();
}

/* ---------- CHANGE PASSWORD ---------- */
void ChangePassword(void)
{
    if(!AuthenticateUser()) return;
    SetPassword_FirstTime();
}

/* ---------- SET TIME ---------- */
void SetTimeout_First(void)
{
   

    LCD_I2C_Clear();
    LCD_I2C_WriteString("Set Time: #:save");

    while(1)
    {
        autoLockTime = POT_ReadMapped(5,30);
        LCD_I2C_SetCursor(1,0);

        char buf[16];
        sprintf(buf,"%02d sec ", autoLockTime);
        LCD_I2C_WriteString(buf);

        if(Keypad_Read()=='#')
        {
            UART1_SendChar('T');
            UART1_SendChar(autoLockTime);
            LCD_I2C_Clear();
            LCD_I2C_WriteString("Saved!");
            delayms(1000);
            return;
        }
    }
}
void ChangeTimeout(void)
{
   if(!AuthenticateUser()) return;
   SetTimeout_First();
}

/* ---------- MENU ---------- */
void Menu(void)
{
    LCD_I2C_Clear();
    LCD_I2C_WriteString("1 Open 2 Chg");
    LCD_I2C_SetCursor(1,0);
    LCD_I2C_WriteString("3 Timeout");

    uint8_t k;
    while(1)
{
    k = Keypad_Read();
    if(k) break;
}
   
    if(k=='1') OpenDoor();
    else if(k=='2') ChangePassword();
    else if(k=='3') ChangeTimeout();
}

/* ---------- MAIN ---------- */
int main(void)
{
    SYSTICK_init(16000);
    LCD_I2C_Init();
    Keypad_Init();
    UART1_Init();
    POT_Init();
    RGB_Init();

    Door_Locked();
    delayms(200);

    uint8_t hasPass = 'X';      
    while(hasPass!='0' && hasPass!='1')
    {
        UART1_SendChar('Q');
        hasPass = UART1_ReceiveChar();
    }

    if(hasPass=='0')
    {
        SetPassword_FirstTime();
        SetTimeout_First();
    }
    else if(hasPass=='1')
    {
        UART1_SendChar('R');
        LoadPasswordFromControl();  // load last saved password
    }

    while(1)
    {
        Menu();
        delayms(200);
    }
}
