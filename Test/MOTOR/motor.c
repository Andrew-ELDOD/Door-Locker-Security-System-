

#include "motor.h"
#include "dio.h"


#define MOTOR_PORT      PORTF
#define MOTOR_IN1       PIN0
#define MOTOR_IN2       PIN4

void Motor_Init(void) {
    /* Initialize IN1 (PF0) as output */
    DIO_Init(MOTOR_PORT, MOTOR_IN1, OUTPUT);
    /* Initialize IN2 (PF4) as output */
    DIO_Init(MOTOR_PORT, MOTOR_IN2, OUTPUT);
    /* Start with motor stopped (both pins LOW) */
    DIO_WritePin(MOTOR_PORT, MOTOR_IN1, LOW);
    DIO_WritePin(MOTOR_PORT, MOTOR_IN2, LOW);
}

void Motor_RotateCW(void) {
    DIO_WritePin(MOTOR_PORT, MOTOR_IN1, HIGH);
    DIO_WritePin(MOTOR_PORT, MOTOR_IN2, LOW);
}

void Motor_RotateCCW(void) {
    DIO_WritePin(MOTOR_PORT, MOTOR_IN1, LOW);
    DIO_WritePin(MOTOR_PORT, MOTOR_IN2, HIGH);
}


void Motor_Stop(void) {
    DIO_WritePin(MOTOR_PORT, MOTOR_IN1, LOW);
    DIO_WritePin(MOTOR_PORT, MOTOR_IN2, LOW);
}
