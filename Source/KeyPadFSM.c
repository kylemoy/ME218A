//#define TEST
/****************************************************************************
 Module
   KeyPadFSM.c

 Revision
   1.0.1

 Description
   This is a KeyPadFSM file for implementing flat state machines under the
   Gen2 Events and Services Framework.

 Notes
****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "KeyPadFSM.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "ES_Port.h"
#include "termio.h"
#include "inc/hw_memmap.h" 
#include "inc/hw_types.h" 
#include "inc/hw_gpio.h" 
#include "inc/hw_sysctl.h" 
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "utils/uartstdio.h"
#include "EnablePA25_PB23_PD7_PF0.h"
#include "LCD.h"

/*----------------------------- Module Defines ----------------------------*/
#define BIT(x) >>(x-1)
#define NUM_ROWS 4
#define NUM_COLS 3
#define clrScrn() 	printf("\x1b[2J")
#define goHome()	printf("\x1b[1,1H")
#define clrLine()	printf("\x1b[K")
#define ALL_BITS (0xFF<<2)
#define SCALE_DELAY (uint16_t)6
#define KEYPAD_TIMER_DELAY 50

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine.They should be functions
   relevant to the behavior of this state machine
*/
void InitKeyPadPortLines(void);
void delayShort(uint16_t delayTime);
void delayLong(uint16_t delayTime);
uint8_t findNum(void);
bool CheckForKeyPadButtonPress(void);
void setCol(int c, int dir);
uint8_t readRow(int r);
void setColsHigh(void);

/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, you may need others as well.
// type of state variable should match htat of enum in header file
static KeyPadFSMState_t CurrentState;
static uint8_t prevButtonState = 0;
static uint8_t Row = 255;
static uint8_t Col = 255;

// with the introduction of Gen2, we need a module level Priority var as well
static uint8_t MyPriority;


/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitKeyPadFSM

 Parameters
     uint8_t : the priorty of this service

 Returns
     bool, false if error in initialization, true otherwise

 Description
     Saves away the priority, sets up the initial transition and does any
     other required initialization for this state machine
 Notes

 Author
     J. Edward Carryer, 10/23/11, 18:55
****************************************************************************/
bool InitKeyPadFSM ( uint8_t Priority )
{
	puts("InitKeyPadFSM\n\r");
	InitKeyPadPortLines();
	
  ES_Event ThisEvent;

  MyPriority = Priority;
  // put us into the Initial PseudoState
  CurrentState = InitPState;
  // post the initial transition event
  ThisEvent.EventType = ES_INIT;
  if (ES_PostToService( MyPriority, ThisEvent) == true)
  {
      return true;
  }else
  {
      return false;
  }
}

/****************************************************************************
 Function
     PostKeyPadFSM

 Parameters
     EF_Event ThisEvent , the event to post to the queue

 Returns
     boolean False if the Enqueue operation failed, True otherwise

 Description
     Posts an event to this state machine's queue
 Notes

 Author
     J. Edward Carryer, 10/23/11, 19:25
****************************************************************************/
bool PostKeyPadFSM( ES_Event ThisEvent )
{
	puts("PostKeyPadFSM\n\r");
  return ES_PostToService( MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunKeyPadFSM

 Parameters
   ES_Event : the event to process

 Returns
   ES_Event, ES_NO_EVENT if no error ES_ERROR otherwise

 Description
   add your description here
 Notes
   uses nested switch/case to implement the machine.
 Author
   J. Edward Carryer, 01/15/12, 15:23
****************************************************************************/
ES_Event RunKeyPadFSM( ES_Event ThisEvent )
{
	//printf("RunKeyPadFSM\n\r");
	//printf("(row, col) = (%i, %i)\n\r", Row, Col);
  ES_Event ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors
	if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == KEYPAD_TIMER))
	{
		uint8_t num = findNum();
		if(num <= 12){
			printf("%i\n\r", num);
			LCDputchar((char)num);
		}
	}
	
  return ReturnEvent;
}

/****************************************************************************
 Function
     QueryKeyPadFSM

 Parameters
     None

 Returns
     KeyPadFSMState_t The current state of the KeyPadFSM state machine

 Description
     returns the current state of the KeyPadFSM state machine
 Notes

****************************************************************************/
KeyPadFSMState_t QueryKeyPadFSM ( void )
{
   return(CurrentState);
}

/*
 * Function reads rows and colums of Keypad and
 * returns number of button pressed on KeyPad
 */
bool CheckForKeyPadButtonPress(void)
{
	//puts("CheckForKeyPadButtonPress\n\r");
	for (int r = 1; r <= NUM_ROWS ; r++){
		for (int c = 1; c <= NUM_COLS; c++){
			setColsHigh();
			setCol(c,0); // set column in question to Low
			uint8_t thisRow = readRow(r);
				if (thisRow && prevButtonState != 1)
				{
					// One of the key pad buttons was pressed. 
					// Post Event ButtonPressed.
					Row = r;
					Col = c;
					//ES_Event ThisEvent;
					//ThisEvent.EventType = ButtonPressed; 
					//PostKeyPadFSM(ThisEvent);
					ES_Timer_InitTimer(KEYPAD_TIMER, KEYPAD_TIMER_DELAY);
					
					prevButtonState = thisRow;;
					return true;
				}
		}
	}
	prevButtonState = 0;
	return false;
}

/***************************************************************************
 private functions
 ***************************************************************************/

/*
 *  *** EDIT HERE FOR SPECIFIC PIN ASSIGNMENTS **
 *
 * Initialize port lines for KeyPad
 * row/col#	|	KeyPad pin - Tiva Pin
 * ---------------------------
 *	col 1		|		pin3 		- 	PC4
 *	col 2		|		pin1		-	 	PC5
 *	col 3		|		pin5 		-	 	PC6		
 * 	row 1		|		pin2 		- 	PC7
 *	row 2		|		pin7 		- 	PD6
 *	row 3		|		pin6 		-	 	PD7
 *	row 4		|		pin4 		-	 	PF4
 */
void InitKeyPadPortLines(void)
{
	PortFunctionInit(); // set PA2-5, PB2-3, PD7, PF0 to GPIO
		
	// wait a few	clock cycles
	uint8_t pause = HWREG(SYSCTL_RCGCGPIO); 
	
	/* Enable Ports on the Tiva
	 * Port F | Port E | Port D | Port C | Port B | Port A
	 * Bit 5	|	Bit 4  | Bit 3  | Bit 2  | Bit 1  | Bit 0 
	 * example: HWREG(SYSCTL_RCGCGPIO) |= BIT1HI; // Enable GPIO Port B
	 */

	HWREG(SYSCTL_RCGCGPIO) |= BIT2HI; // Enable GPIO Port C
	HWREG(SYSCTL_RCGCGPIO) |= BIT3HI; // Enable GPIO Port D
	HWREG(SYSCTL_RCGCGPIO) |= BIT5HI; // Enable GPIO Port F
	
	// wait a few	clock cycles after enabling clock
	pause = HWREG(SYSCTL_RCGCGPIO); 
	
	/* Set Port pins to digital or analog function
	 * GPIO_O_DEN bit: 1 = Digital, 0 = Analog
	 */
	HWREG(GPIO_PORTC_BASE+GPIO_O_DEN) |= (GPIO_PIN_4)|(GPIO_PIN_5)|(GPIO_PIN_6)|(GPIO_PIN_7);// set Port C pins 4-7 to digital function
	HWREG(GPIO_PORTD_BASE+GPIO_O_DEN) |= (GPIO_PIN_6)|(GPIO_PIN_7);// set Port D pins 6,7 to digital function
	HWREG(GPIO_PORTF_BASE+GPIO_O_DEN) |= (GPIO_PIN_4);// set Port F pin 4 to digital function
	
	/* Set pin directions to Intput or Output
	 * GPIO_O_DIR bit: 1 = Output, 0 = Input
	 */
	 //rows as inputs
	HWREG(GPIO_PORTC_BASE+GPIO_O_DIR) &= ~(GPIO_PIN_7);// set Port C pin 7 to input
	HWREG(GPIO_PORTD_BASE+GPIO_O_DIR) &= ~((GPIO_PIN_6)|(GPIO_PIN_7));// set Port D pin 6,7 to input
	HWREG(GPIO_PORTF_BASE+GPIO_O_DIR) &= ~(GPIO_PIN_4);// set Port F pin 4 to input
	
	// columns as outputs
	HWREG(GPIO_PORTC_BASE+GPIO_O_DIR) |= ((GPIO_PIN_4)|(GPIO_PIN_5)|(GPIO_PIN_6));// set Port C pins 4-6 to output

	// Enable pull-up resistors on all rows (NOTE: Not valid on input pins)
	//HWREG(GPIO_PORTC_BASE + GPIO_O_PUR) |= (GPIO_PIN_7);
	//HWREG(GPIO_PORTD_BASE + GPIO_O_PUR) |= ((GPIO_PIN_6)|(GPIO_PIN_7));
	//HWREG(GPIO_PORTF_BASE + GPIO_O_PUR) |= (GPIO_PIN_4);

	setColsHigh();

	printf("Done Initializing KeyPad Port Lines\n\r");
}

/*
 *  *** EDIT HERE FOR SPECIFIC PIN ASSIGNMENTS **
 * Set Column High or Low
 */
void setCol(int c, int dir) // sets column C as HI (dir ==1) or Lo.
{
	switch(c)
	{
		case 1: // column 1
			if (dir == 1)
				HWREG(GPIO_PORTC_BASE+(GPIO_O_DATA + ALL_BITS)) |= (GPIO_PIN_4);
			else 
				HWREG(GPIO_PORTC_BASE+(GPIO_O_DATA + ALL_BITS)) &= ~(GPIO_PIN_4);
			break;
		
		case 2: // column 2
			if (dir == 1)
				HWREG(GPIO_PORTC_BASE+(GPIO_O_DATA + ALL_BITS)) |= (GPIO_PIN_5);
			else
				HWREG(GPIO_PORTC_BASE+(GPIO_O_DATA + ALL_BITS)) &= ~(GPIO_PIN_5);
			break;
			
		case 3: // column 3
			if (dir == 1)
				HWREG(GPIO_PORTC_BASE+(GPIO_O_DATA + ALL_BITS)) |= (GPIO_PIN_6);
			else
				HWREG(GPIO_PORTC_BASE+(GPIO_O_DATA + ALL_BITS)) &= ~(GPIO_PIN_6);
			break;
			
		default:
			break;
	}
}

/*
 *  *** EDIT HERE FOR SPECIFIC PIN ASSIGNMENTS **
 * Read state of a row
 * eg. calling row(1) returns 1 if the first row
 * sees a button press, 0 otherwise.
 *
 */
uint8_t readRow(int r)
{
	switch (r)
	{
		case 1: // row 1
			if ((HWREG(GPIO_PORTC_BASE+(GPIO_O_DATA + ALL_BITS)) &= (GPIO_PIN_7)) == 0) ///basicalyl if port C7 is low, then it is true
				return 1;
			else 
				return 0;
	
		case 2: // row 2
			if ((HWREG(GPIO_PORTD_BASE+(GPIO_O_DATA + ALL_BITS)) &= (GPIO_PIN_6)) == 0)
				return 1;
			else 
				return 0;
	
		case 3: // row 3
			if ((HWREG(GPIO_PORTD_BASE+(GPIO_O_DATA + ALL_BITS)) &= (GPIO_PIN_7)) == 0)
				return 1;
			else 
				return 0;
	
		case 4: // row 4 
			if ((HWREG(GPIO_PORTF_BASE+(GPIO_O_DATA + ALL_BITS)) &= (GPIO_PIN_4)) == 0)
				return 1;
			else 
				return 0;
	
		default:
			return 0;
	}
}

/*
 * Function takes a Row and Column 
 * and returns the value of the number at the 
 * intersection of that row and column, 
 * assuming a 4-row x 3-column keypad layout.
 */
uint8_t findNum(void)
{
	uint8_t num = (Row-1)*NUM_COLS + Col;
		if (num == 11)
			return 0;
		else if ((num >= 1) && (num <= 9))
			return num;
		else
			return 255;
}

/* 
 * Set all columns high
 */
void setColsHigh() // setting all columns HI
{
	for (int c = 1 ; c  <=NUM_COLS; c++)
	{
		setCol(c, 1);
	}
}

/*
 * Pause number of microseconds (us).
 * Will pause a little longer than input time.
 */
void delayShort(uint16_t delayTime)
{
	uint16_t scaledTime = delayTime*SCALE_DELAY;
	for(int i=0; i<scaledTime; i++)
	{
		uint8_t pause = HWREG(SYSCTL_RCGCGPIO);
	}
}

/*
 * Pause number of milliseconds (ms).
 * Will pause a little longer than input time.
 */
void delayLong(uint16_t delayTime)
{
	uint16_t currentTime = ES_Timer_GetTime();
	uint16_t targetTime = ES_Timer_GetTime() + delayTime;
	while (currentTime != targetTime){
			currentTime = ES_Timer_GetTime();
	}
}

/***************************************************************************
											TEST HARNESS 
***************************************************************************/
#ifdef TEST 
/* test Harness for testing this module */ 
#include "termio.h" 
int main(void) 
{ 
	// Set the clock to run at 40MhZ using the PLL and 16MHz external crystal
	SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN
			| SYSCTL_XTAL_16MHZ);
	_HW_Timer_Init(ES_Timer_RATE_1mS);
	TERMIO_Init();
	clrScrn();

	ES_Return_t ErrorType;


	// When doing testing, it is useful to announce just which program
	// is running.
	puts("\rStarting Test Harness for \r");
	printf("the 2nd Generation Events & Services Framework V2.2\r\n");
	printf("%s %s\n",__TIME__, __DATE__);
	printf("\n\r\n");
	printf("Press any key to post key-stroke events to Service 0\n\r");
	printf("Press 'd' to test event deferral \n\r");
	printf("Press 'r' to test event recall \n\r");

	// Your hardware initialization function calls go here

	// now initialize the Events and Services Framework and start it running
	ErrorType = ES_Initialize(ES_Timer_RATE_1mS);
	if ( ErrorType == Success ) {

	  ErrorType = ES_Run();

	}
	//if we got to here, there was an error
	switch (ErrorType){
	  case FailedPost:
	    printf("Failed on attempt to Post\n");
	    break;
	  case FailedPointer:
	    printf("Failed on NULL pointer\n");
	    break;
	  case FailedInit:
	    printf("Failed Initialization\n");
	    break;
	 default:
	    printf("Other Failure\n");
	    break;
	}
	for(;;)
	  ;
	return 0; 
} 
#endif 
