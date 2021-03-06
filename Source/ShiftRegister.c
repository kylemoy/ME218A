/****************************************************************************
 Module
   ShiftRegister.c

 Description
   This file contains the function that will control the shift register data
	 inputs, as well as the RCK/SCK clocks. It will take an ARRAY of 8 numbers
	 0 or 1 that will correspond to the 8 LEDs
****************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_sysctl.h"
#include "driverlib/sysctl.h"
#include "ES_Port.h"
#include "ES_Timers.h"
#include "EnablePA25_PB23_PD7_PF0.h"
#include "termio.h"

// not as sure if these are absolutley needed
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "utils/uartstdio.h"

#define ALL_BITS (0xff <<2)
//#define TEST

/****************************************************************************
  Change these values if ports A2 (data), A3 (RCK), A4 (SCK) are
	not being used for the shift register
****************************************************************************/
#define SHIFT_PORT_DEC SYSCTL_RCGCGPIO_R0 //port A
#define SHIFT_PORT GPIO_PORTA_BASE // port A base
#define SHIFT_DATA GPIO_PIN_2 // pin 2
#define SHIFT_SCK GPIO_PIN_3 // pin 3
#define SHIFT_RCK GPIO_PIN_4 // pin 4

void ShiftRegInit (void);
void ShiftRegLED (char LEDs[8]);
void SCKPulse (void); // transfer bit to shift register
void RCKPulse (void); // transfer bits to the world
void wait (int delay); // a wait function

void wait (int delay) {
	int time_start = ES_Timer_GetTime(); 
	int time = ES_Timer_GetTime();
	
	while (time < time_start + delay) {
		HWREG(SYSCTL_RCGCGPIO);
		time = ES_Timer_GetTime();
	}
}

void ShiftRegInit (void) {
	// initializes timer
	_HW_Timer_Init(ES_Timer_RATE_1mS);
	
	// pin initialization to foolow
	PortFunctionInit(); // seems to initialize all the pins PA2-5, PB2-3, PD7, PF0 as GPIO
	
	//initialization - should be moved to main code later
	HWREG(SYSCTL_RCGCGPIO) |= (SHIFT_PORT_DEC); // enable port A
	HWREG(SHIFT_PORT + GPIO_O_DEN) |= (SHIFT_DATA | SHIFT_SCK | SHIFT_RCK); //enables pins
	HWREG(SHIFT_PORT + GPIO_O_DIR) |= (SHIFT_DATA | SHIFT_SCK | SHIFT_RCK); // makes pins as ouputs
	
	//writes pins LO to make sure it starts out LO (good for pulsing)
	HWREG(SHIFT_PORT+(GPIO_O_DATA + ALL_BITS)) &= ~(SHIFT_DATA | SHIFT_SCK | SHIFT_RCK); 
	
	puts("\n\r Timer, Shift Register port and pins intialization complete \r\n");
}

void SCKPulse (void) {
	// pulses SCK pin HI
	HWREG(SHIFT_PORT+(GPIO_O_DATA + ALL_BITS)) |= SHIFT_SCK; 
	//kill time
	wait(5);
	// pulses SCK pin LO
	HWREG(SHIFT_PORT+(GPIO_O_DATA + ALL_BITS)) &= ~SHIFT_SCK; 
}

void RCKPulse (void) {
	// pulses RCK pin HI
	HWREG(SHIFT_PORT+(GPIO_O_DATA + ALL_BITS)) |= SHIFT_RCK; 
	//kill time
	wait(2);
	// pulses SCK pin LO
	HWREG(SHIFT_PORT+(GPIO_O_DATA + ALL_BITS)) &= ~SHIFT_RCK; 
	
}

void ShiftRegLED (char LEDs[8]) {
	for (int i = 0; i < 8; i++) {
		int bit = 0;
		//clears data pin
		HWREG(SHIFT_PORT + GPIO_O_DATA + ALL_BITS) &= ~SHIFT_DATA;
		
		// writes the relavant bit to data pin
		bit = LEDs[i];
		
		if (bit == 1) {
			HWREG(SHIFT_PORT + GPIO_O_DATA + ALL_BITS) |= SHIFT_DATA;
		}
		else if (bit == 0) {
			HWREG(SHIFT_PORT + GPIO_O_DATA + ALL_BITS) &= ~SHIFT_DATA;
		}
		
		printf("\n\r for i counter %d\n\r", i);
		printf("\n\r Shift Register Data input is %c \r\n", LEDs[i]);
		printf("\n\r the bit was seen as %d \r\n", bit);
		
		// pulses SCK to send data to shift register
		SCKPulse();
	}
	
	// leaving the loop means it has gone through all 8
	RCKPulse();
}

#ifdef TEST 
/* test Harness for testing this module */ 

int main(void) 
{ 
	TERMIO_Init(); 
	puts("\n\r in test harness for shift register\r\n");
	
	char test[8] =  {0,0,0,0,0,0,0,1};

	ShiftRegInit();
	ShiftRegLED (test);
}
#endif
