#include "tm4c123gh6pm.h"
#include "uart.h"
#include "eeprom.h"
#include "SYSTICK.h"
#include "dio.h"
#include <string.h>
#include "motor.h"
#include "buzzer.h"

/* -------- Config -------- */
#define PASSWORD_LEN        5
#define MAX_TRIES           3
#define MOTOR_DELAY_MS       100U
#define AUTOLOCK_MULTIPLIER 1000U

#define EEPROM_PASS_ADDR    0
#define EEPROM_TIME_ADDR    6
#define EEPROM_FLAG_ADDR    10
#define EEPROM_MAGIC_ADDR   20
#define EEPROM_MAGIC_VALUE  0x7  

char stored_password[PASSWORD_LEN + 1];
uint8_t passwordValid   = 0;
uint8_t autoLockTime    = 10;
uint8_t failedAttempts  = 0;

/* -------- EEPROM -------- */
void EEPROM_FirstBootCheck(void)
{
    uint8_t magic;
    EEPROM_ReadBuffer(0, EEPROM_MAGIC_ADDR, &magic, 1);

    if(magic != EEPROM_MAGIC_VALUE)
    {
        uint8_t zero = 0;
        uint8_t defaultTime = 10;
        static const uint8_t defaultPassword[PASSWORD_LEN] = {'0','0','0','0','0'}; 
        EEPROM_WriteBuffer(0, EEPROM_PASS_ADDR, defaultPassword, PASSWORD_LEN);

        EEPROM_WriteBuffer(0, EEPROM_FLAG_ADDR, &zero, 1);
        EEPROM_WriteBuffer(0, EEPROM_TIME_ADDR, &defaultTime, 1);

        magic = EEPROM_MAGIC_VALUE;
        EEPROM_WriteBuffer(0, EEPROM_MAGIC_ADDR, &magic, 1);
    }
}

void LoadConfig(void)
{
    EEPROM_ReadBuffer(0, EEPROM_FLAG_ADDR, &passwordValid, 1);

    if(passwordValid)
    {
        EEPROM_ReadBuffer(0, EEPROM_PASS_ADDR,(uint8_t*)stored_password, PASSWORD_LEN);
        stored_password[sizeof(stored_password)/sizeof(stored_password[0]) - 1] = '\0';
    }

    EEPROM_ReadBuffer(0, EEPROM_TIME_ADDR, &autoLockTime, 1);
    if(autoLockTime < 5 || autoLockTime > 30)
        autoLockTime = 10;
}

void SavePassword(char *pass)
{
    EEPROM_WriteBuffer(0, EEPROM_PASS_ADDR, (uint8_t*)pass, PASSWORD_LEN);
    passwordValid = 1;
    EEPROM_WriteBuffer(0, EEPROM_FLAG_ADDR, &passwordValid, 1);
    (void)memcpy(stored_password, pass, PASSWORD_LEN);
    stored_password[sizeof(stored_password)/sizeof(stored_password[0]) - 1] = '\0';

}

/* -------- Door -------- */
void Door_Open_Close(void)
{
    Motor_RotateCW();
    delayms(MOTOR_DELAY_MS);
    Motor_Stop();

    delayms(autoLockTime * AUTOLOCK_MULTIPLIER);

    Motor_RotateCCW();
    delayms(MOTOR_DELAY_MS);
    Motor_Stop();
}

/* -------- MAIN -------- */
int main(void)
{
    SYSTICK_init(16000);
    UART1_Init();
    EEPROM_Init();
    Motor_Init();
    Buzzer_Init();

    EEPROM_FirstBootCheck();
    LoadConfig();

    uint8_t cmd;
    char rx[PASSWORD_LEN+1];
    uint8_t i;


    while(1)
    {
        if(UART1_IsDataAvailable())
        {
            cmd = UART1_ReceiveChar();

            if(cmd == 'Q')   // Query password exist
            {
                UART1_SendChar(passwordValid ? '1' : '0');
            }

            else if(cmd == 'S')   // First Set
            {
                for(i=0;i<PASSWORD_LEN;i++)
                    rx[i] = UART1_ReceiveChar();
                rx[PASSWORD_LEN] = '\0';
                SavePassword(rx);
            }

            else if(cmd == 'C')   // Change
            {
                for(i=0;i<PASSWORD_LEN;i++)
                    rx[i] = UART1_ReceiveChar();
                rx[PASSWORD_LEN] = '\0';
                SavePassword(rx);
                UART1_SendChar('1');
            }

            else if(cmd == 'O')   // Open
            {
                if(!passwordValid)
                {
                    UART1_SendChar('0');
                    continue;
                }

                for(i=0;i<PASSWORD_LEN;i++)
                    rx[i] = UART1_ReceiveChar();
                rx[PASSWORD_LEN] = '\0';

                if(strcmp(rx, stored_password) == 0)
                {
                    UART1_SendChar('1');
                    failedAttempts = 0;
                    Door_Open_Close();
                }
                else
                {
                    failedAttempts++;
                    UART1_SendChar('0');
                    if(failedAttempts >= MAX_TRIES)
                        Buzzer_Beep(5);
                }
            }
            else if(cmd == 'A')   // Open
            {
                if(!passwordValid)
                {
                    UART1_SendChar('A');
                    continue;
                }

                for(i=0;i<PASSWORD_LEN;i++)
                    rx[i] = UART1_ReceiveChar();
                rx[PASSWORD_LEN] = '\0';

                if(strcmp(rx, stored_password) == 0)
                {
                    UART1_SendChar('1');
                    failedAttempts = 0;
                }
                else
                {
                    failedAttempts++;
                    UART1_SendChar('0');
                    if(failedAttempts >= MAX_TRIES)
                        Buzzer_Beep(5);
                }
            }

            else if(cmd == 'T')   // Timeout
            {
                autoLockTime = UART1_ReceiveChar();
                EEPROM_WriteBuffer(0, EEPROM_TIME_ADDR,
                                   &autoLockTime, 1);
            }

            else if(cmd == 'L')   // Lockout
            {
                failedAttempts = MAX_TRIES;
                Buzzer_Beep(5);
            }
        }
    }
}
