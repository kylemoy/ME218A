/****************************************************************************
 Module
   DCMotors.c

 Description
   This file contains the function that will control the DC motors for 
	 vibration motor or feather/ball dropper.
****************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_sysctl.h"
#include "driverlib/sysctl.h"
#include "termio.h"
#include "ES_Port.h"

// not as sure if these are absolutley needed
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "utils/uartstdio.h"

#define ALL_BITS (0xff <<2)
//#define TEST

/****************************************************************************
  Port A4 (on/off) for Vibration Motor
	Port A5 (on/off) and A6 (direction) for Timing Motor
****************************************************************************/
#define MOTOR_PORT_DEC SYSCTL_RCGCGPIO_R0 //port A
#define MOTOR_PORT GPIO_PORTA_BASE // port A base

#define VIB_MOTOR_PIN GPIO_PIN_5 // pin A5
#define TIME_MOTOR_PIN GPIO_PIN_6 // pin A6
#define TIME_MOTOR_DIRECTION_PIN GPIO_PIN_7 // pin A7

#define VIB_MOTOR_ON BIT5HI
#define TIME_MOTOR_ON BIT6HI
#define TIME_MOTOR_FORWARD BIT7HI

#define ON 1
#define OFF 0
#define FORWARD 1
#define BACKWARD 0


void initMotors(void) {
	// Port initializations
	HWREG(SYSCTL_RCGCGPIO) |= (MOTOR_PORT_DEC); // Enable port A
	HWREG(MOTOR_PORT+GPIO_O_DEN) |= (VIB_MOTOR_PIN | TIME_MOTOR_PIN | TIME_MOTOR_DIRECTION_PIN); // Set ports to be Digital IO
	HWREG(MOTOR_PORT+GPIO_O_DIR) |= (VIB_MOTOR_PIN | TIME_MOTOR_PIN | TIME_MOTOR_DIRECTION_PIN); // Set ports to be Outputs
	HWREG(MOTOR_PORT+(GPIO_O_DATA + ALL_BITS)) &= ~(VIB_MOTOR_PIN | TIME_MOTOR_PIN | TIME_MOTOR_DIRECTION_PIN); // Set ports to low state
}

void setMotor(int motor, int state) {
	switch (motor) {
		case VIB_MOTOR_PIN:
			if (state == ON)
				HWREG(MOTOR_PORT+(GPIO_O_DATA + ALL_BITS)) |= (VIB_MOTOR_PIN);
			else
				HWREG(MOTOR_PORT+(GPIO_O_DATA + ALL_BITS)) &= ~(VIB_MOTOR_PIN);
			
		case TIME_MOTOR_PIN:
			if (state == ON)
				HWREG(MOTOR_PORT+(GPIO_O_DATA + ALL_BITS)) |= (TIME_MOTOR_ON);
			else
				HWREG(MOTOR_PORT+(GPIO_O_DATA + ALL_BITS)) &= ~(TIME_MOTOR_ON);
			
		case TIME_MOTOR_DIRECTION_PIN:
			if (state == FORWARD)
				HWREG(MOTOR_PORT+(GPIO_O_DATA + ALL_BITS)) |= (TIME_MOTOR_FORWARD);
			else
				HWREG(MOTOR_PORT+(GPIO_O_DATA + ALL_BITS)) &= ~(TIME_MOTOR_FORWARD);	
	}	
}

bool getMotorState(int motor) {
	switch (motor) {
		case VIB_MOTOR_PIN:
			return ((HWREG(MOTOR_PORT+(GPIO_O_DATA + ALL_BITS)) & VIB_MOTOR_PIN) == VIB_MOTOR_ON);
		case TIME_MOTOR_PIN:
			return ((HWREG(MOTOR_PORT+(GPIO_O_DATA + ALL_BITS)) & TIME_MOTOR_PIN) == TIME_MOTOR_ON);
		case TIME_MOTOR_DIRECTION_PIN:
			return ((HWREG(MOTOR_PORT+(GPIO_O_DATA + ALL_BITS)) & TIME_MOTOR_DIRECTION_PIN) == TIME_MOTOR_FORWARD);
	}
	return true;
}
	

#ifdef TEST 
/* Test Harness for DC motor module */ 

int main(void) { 
	TERMIO_Init(); 
	puts("\n\r In Test Harness for DC Motor Module\r\n");
	initMotors();
	while(true){
		char input = getchar();
			switch (input) {
			case 'z':
				if (getMotorState(VIB_MOTOR_PIN) == ON) {
					setMotor(VIB_MOTOR_PIN, OFF);
					printf("Setting Vibration Motor Off\r\n");
				} else {
					setMotor(VIB_MOTOR_PIN, ON);
					printf("Setting Vibration Motor On\r\n");
				}
				break;
			
			case 'x':
				if (getMotorState(TIME_MOTOR_PIN) == ON) {
					setMotor(TIME_MOTOR_PIN, OFF);
					printf("Setting Time Motor Off\r\n");
				} else {
					setMotor(TIME_MOTOR_PIN, ON);
					printf("Setting Time Motor On\r\n");
				}
				break;
			
			case 'c':
				if (getMotorState(TIME_MOTOR_DIRECTION_PIN) == FORWARD) {
					setMotor(TIME_MOTOR_DIRECTION_PIN, BACKWARD);
					printf("Setting Time Motor Backward\r\n");
				} else {
					setMotor(TIME_MOTOR_DIRECTION_PIN, FORWARD);
					printf("Setting Time Motor Forward\r\n");
				}break;
		}
	}
}
#endif
