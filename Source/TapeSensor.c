//#define TEST

/****************************************************************************
 Module
   TapeSensor.c

 Description
   This file contains the function that will read the tape sensor output and
   report its state back
****************************************************************************/
#include "TapeSensor.h"


#define ALL_BITS (0xff <<2)
#define TAPE_PORT_DEC SYSCTL_RCGCGPIO_R3 //port D
#define TAPE_PORT GPIO_PORTD_BASE // port D base
#define TAPE_PIN GPIO_PIN_7 // pin 7
#define TAPE_HI BIT7HI

void initTapeSensors(void) {
	// Initialization of tape sensor port
	HWREG(SYSCTL_RCGCGPIO) |= (TAPE_PORT_DEC); // Enable port F
	HWREG(TAPE_PORT+GPIO_O_DEN) |= TAPE_PIN; // Pin 4
}

bool tapeSensorsCovered(void) {
	return !(HWREG(TAPE_PORT+(GPIO_O_DATA + ALL_BITS)) & TAPE_PIN);
}

#ifdef TEST 
/* Test Harness for Tape Sensors module */ 
int main(void) 
{ 
	TERMIO_Init(); 
	printf("\n\rIn Test Harness for Tape Sensors\r\n");
	initTapeSensors();
	while(true){
		if (tapeSensorsCovered()) {
			printf("All three tape sensors are covered.\r\n");
		} else {
			printf("One or more tape sensors are not covered.\r\n");
		}
	}
}
#endif
