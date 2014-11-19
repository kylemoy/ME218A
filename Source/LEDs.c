/****************************************************************************
 Module
   LEDs.c

 Description
   This file contains the function that will control the LED array using the shift 
	 register data inputs, as well as the RCK/SCK clocks. It will take an ARRAY of 
	 8 numbers, 0 or 1 that will correspond to the 8 LEDs
****************************************************************************/
#include "LEDs.h"
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

#define ALL_BITS (0xff <<2)


void LEDShiftRegInit (void) {
	// initializes timer
	_HW_Timer_Init(ES_Timer_RATE_1mS);
	
	// pin initialization to foolow

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

void setLED (char LEDs[8]) {
	printf(" LED: ");
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
		
		printf("%d", bit);
		
		// pulses SCK to send data to shift register
		SCKPulse();
	}
	printf("\r\n");
	
	// leaving the loop means it has gone through all 8
	RCKPulse();
}

#ifdef TEST 
/* test Harness for testing this module */ 

#define Tier1 2
#define Tier2 0
#define Tier3 7
#define Tier4 1
#define Tier5 4
#define Tier6 5
#define pot 3

int main(void) 
{ 
	TERMIO_Init(); 
	puts("\n\r in test harness for shift register\r\n");
	
	// 0 is On, 1 is Off

	// Array index 6 is empty (no LED)
	char test[8] =  {1,1,1,1,1,1,1,1};
	test[Tier1] = 0;
	test[pot] = 0;
	LEDShiftRegInit();
	setLED (test);
}
#endif
