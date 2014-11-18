//#define TEST

/****************************************************************************
 Module
   SlotDetector.c

 Description
   This file contains the function that will read the phototransistor output
   to test for the presence of a key in the slot.
****************************************************************************/
#include "SlotDetector.h"

#define ALL_BITS (0xff <<2)
#define PHOTOTRANS_PORT_DEC SYSCTL_RCGCGPIO_R1 //port B
#define PHOTOTRANS_PORT GPIO_PORTB_BASE
#define PHOTOTRANS_PIN GPIO_PIN_2 // pin 2
#define PHOTOTRANS_HI BIT2HI

int keyInSlotDefault = 0;


void setKeyInSlotDefault(void) {
	keyInSlotDefault = isKeyInSlot();
}

int getKeyInSlotDefault(void) {
	return keyInSlotDefault;
}

void initPhototransistor(void) {
	// Initialization of tape sensor port
	HWREG(SYSCTL_RCGCGPIO) |= (PHOTOTRANS_PORT_DEC);
	HWREG(PHOTOTRANS_PORT+GPIO_O_DEN) |= (PHOTOTRANS_PIN); 
}

bool isKeyInSlot(void) {
	return !(HWREG(PHOTOTRANS_PORT+(GPIO_O_DATA + ALL_BITS)) & PHOTOTRANS_HI);
}


#ifdef TEST 
/* Test Harness for Slot Detector module */ 
int main(void) 
{ 
	TERMIO_Init(); 
	printf("\n\rIn Test Harness for Slot Detector\r\n");
	initTapeSensors();
	while(true){
		if (isKeyInSlot()) {
			printf("Slot is covered.\r\n");
		} else {
			printf("Slot is not.\r\n");
		}
	}
	break;
}
#endif
