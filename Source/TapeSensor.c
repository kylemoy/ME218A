/****************************************************************************
 Module
   TapeSensor.c

 Description
   This file contains the function that will read the tape sensor output and
   report its state back
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
  Change these values if port F 2,3,4 is not used for the tape reading
****************************************************************************/
#define TAPE_PORT_DEC SYSCTL_RCGCGPIO_R5 //port F
#define TAPE_PORT GPIO_PORTF_BASE // port F base
#define TAPE_PIN1 GPIO_PIN_4 // pin 4
#define TAPE1_HI BIT4HI
#define TAPE_PIN2 GPIO_PIN_2 // pin 2
#define TAPE2_HI BIT2HI
#define TAPE_PIN3 GPIO_PIN_3 // pin 3
#define TAPE3_HI BIT3HI

bool TapeRead (void);

bool TapeRead(void) {
	bool tapes = false;
	char Tape1 = 0;
	char Tape2 = 0;
	char Tape3 = 0;
	
	// initialization of ports to be moved to main code when available
	HWREG(SYSCTL_RCGCGPIO) |= (TAPE_PORT_DEC); // enable port F
	HWREG(TAPE_PORT+GPIO_O_DEN) |= (TAPE_PIN1|TAPE_PIN2|TAPE_PIN3); //pin 4, 2, and 3
	
	//Reads tape sensors - pin 4, 2 and 3
	Tape1 = (HWREG(TAPE_PORT+(GPIO_O_DATA + ALL_BITS)) & TAPE_PIN1);
	//Tape2 = (HWREG(TAPE_PORT+(GPIO_O_DATA + ALL_BITS)) & TAPE_PIN2);
	//Tape3 = (HWREG(TAPE_PORT+(GPIO_O_DATA + ALL_BITS)) & TAPE_PIN3);
	
	if (Tape1 == TAPE1_HI) {// && (Tape2 == TAPE2_HI) && (Tape3 == TAPE3_HI)) {
		tapes = false;
		puts("\n\r all three tapes are LOW\r\n");
	} else {
		puts("\n\r all three tapes are HI\r\n");
	}
	return tapes;
}

#ifdef TEST 
/* test Harness for testing this module */ 

int main(void) 
{ 
	TERMIO_Init(); 
	puts("\n\r in test harness for tape sensors \r\n");

	while(1){
		TapeRead();
	}
}
#endif
