//#define TEST

/****************************************************************************
 Module
   TapeSensor.c

 Description
   This file contains the function that will read the tape sensor output and
   report its state back
****************************************************************************/
#include "TapeSensor.h"

/****************************************************************************
  Change these values if port F 2,3,4 is not used for the tape reading
****************************************************************************/
#define ALL_BITS (0xff <<2)
#define TAPE_PORT_DEC SYSCTL_RCGCGPIO_R5 //port F
#define TAPE_PORT GPIO_PORTF_BASE // port F base
#define TAPE_PIN GPIO_PIN_4 // pin 4
#define TAPE_HI BIT4HI

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
