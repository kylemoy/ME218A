//#define TEST 
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
#include "AudioControl.h"

/*----------------------------- Module Defines ----------------------------*/
#define clrScrn() 	printf("\x1b[2J")
#define goHome()	printf("\x1b[1,1H")
#define clrLine()	printf("\x1b[K")
#define ALL_BITS (0xFF<<2)
#define AUDIO_PIN (GPIO_PIN_1)
#define PULSE_WIDTH 50 //in milliseconds
#define MULT_PULSE_SPACING 30 //in milliseconds
#define EVENT_SPACING 200
#define NUM_TRACKS 4

// Track Lengths in milliseconds
#define WOOHOO_LENGTH 180
#define MAMA_MIA_LENGTH 750
#define FINAL_SUCCESS_LENGTH 1200
#define TIMES_UP_LENGTH 2000

/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, you may need others as well.
// type of state variable should match htat of enum in header file
typedef enum {Woohoo, MamaMia, TimesUp, FinalSuccess} track_t;
static track_t lastTrack = Woohoo;


/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine.They should be functions
   relevant to the behavior of this state machine
*/
//void Pause(void);
//void PlayWoohoo(void);
//void PlaySuccessFinish(void);
//void PlayMamaMia(void);
void PlayTimesUp(void);
void moveForward(void);
void moveBackward(void);
void moveToTrack(track_t);
void delayLongAudio(uint16_t delayTime);
void pulse(void);
void InitAudioPortLine(void);
//void AudioInit( void);




/*
 * Pause number of milliseconds (ms).
 * Will pause a little longer than input time.
 */
void delayLongAudio(uint16_t delayTime)
{
	uint16_t currentTime = ES_Timer_GetTime();
	uint16_t targetTime = ES_Timer_GetTime() + delayTime;
	while (currentTime != targetTime){
			currentTime = ES_Timer_GetTime();
	}
}

/*
 * Initialize port lines for Audio
 */
void InitAudioPortLine(void)
{
	PortFunctionInit(); // set PA2-5, PB2-3, PD7, PF0 to GPIO
		
	// wait a few	clock cycles
	uint8_t pause = HWREG(SYSCTL_RCGCGPIO); 

	/* Enable Ports on the Tiva
	 * Port F | Port E | Port D | Port C | Port B | Port A
	 * Bit 5	|	Bit 4  | Bit 3  | Bit 2  | Bit 1  | Bit 0 
	 * example: HWREG(SYSCTL_RCGCGPIO) |= BIT1HI; // Enable GPIO Port B
	 */
	HWREG(SYSCTL_RCGCGPIO) |= BIT4HI; // Enable GPIO Port F
	pause = HWREG(SYSCTL_RCGCGPIO); 
	/* Set Port pins to digital or analog function
	 * GPIO_O_DEN bit: 1 = Digital, 0 = Analog
	 */
	HWREG(GPIO_PORTF_BASE+GPIO_O_DEN) |= (AUDIO_PIN);// set Port F pin 1 to digital function
	/*
	 * Set pin directions to Intput or Output
	 * GPIO_O_DIR bit: 1 = Output, 0 = Input
	 */
	HWREG(GPIO_PORTF_BASE+GPIO_O_DIR) |= (AUDIO_PIN);// set Port F pin 1 to output
}

/* 
 * Function Initializes Audio.
 * Performs an initialization of ports before
 * beginning Audio initialization.
 * 
 * Hardware Pin Mappings
 * LaunchPad Pin -> Audio Pin
 * ------------------------------------------
 * Port F pin 1 -> Audio control pin
 *
 * IMPORTANT; MP3 MUST BEGIN AT 'Woohoo' TRACK!
 */
//void AudioInit( void);
void AudioInit()
{
	InitAudioPortLine();
	printf("Done Initializing Audio \n\r");
}

void Pause(void)
{
 pulse(); 
}

/*
 * Function pulses enable line (Port B pin 7) to perform data transfer
 * Stays high for at least 10us
 */
void pulse(void)
{
	HWREG(GPIO_PORTF_BASE+(GPIO_O_DATA + ALL_BITS)) |= (AUDIO_PIN);
	delayLongAudio(PULSE_WIDTH);
	HWREG(GPIO_PORTF_BASE+(GPIO_O_DATA + ALL_BITS)) &= ~(AUDIO_PIN);
}

void moveForward(void)
{
	pulse();
	delayLongAudio(MULT_PULSE_SPACING);
	pulse();
}

void moveBackward(void)
{
	pulse();
	delayLongAudio(MULT_PULSE_SPACING);
	pulse();
	delayLongAudio(MULT_PULSE_SPACING);
	pulse();
}

void moveToTrack(track_t ThisTrack)
{
	uint8_t distance = 0;
	
	//move to this track
	if (lastTrack == ThisTrack){
		printf("same track\n\r");
		moveBackward();
		delayLongAudio(EVENT_SPACING);
		moveForward();
	}else if(lastTrack > ThisTrack){
		distance = (lastTrack - ThisTrack);
		printf("back distance = %i\n\r", distance);
		for(int i=0; i<distance; i++){
				moveBackward();
				delayLongAudio(EVENT_SPACING);
		}
	}else{
		distance = (ThisTrack - lastTrack);
		printf("forward distance = %i\n\r", distance);
		for(int i=0; i<distance; i++){
				moveForward();
				delayLongAudio(EVENT_SPACING);
		}
	}	
}

/***************************************************
		Functions to play specific tracks are below
****************************************************/

void PlayWohoo(void)
{
	moveToTrack(Woohoo);
	
	delayLongAudio(EVENT_SPACING);
	pulse();//play
	delayLongAudio(WOOHOO_LENGTH);
	pulse();//pause at end of track
	
	lastTrack = Woohoo;//set last track to this track
	delayLongAudio(EVENT_SPACING);
}	

void PlayMamaMia(void)
{
	moveToTrack(MamaMia);
	
	delayLongAudio(EVENT_SPACING);
	pulse();//play
	delayLongAudio(MAMA_MIA_LENGTH);
	pulse();//pause at end of track
	
	lastTrack = MamaMia;//set last track to this track
	delayLongAudio(EVENT_SPACING);
}

void PlayGameOver(void)
{
	moveToTrack(TimesUp);
	
	delayLongAudio(EVENT_SPACING);
	pulse();//play
	delayLongAudio(TIMES_UP_LENGTH);
	pulse();//pause at end of track
	
	lastTrack = TimesUp;//set last track to this track
	delayLongAudio(EVENT_SPACING);
}

void PlaySuccessFinish(void)
{
	moveToTrack(FinalSuccess);
	
	delayLongAudio(EVENT_SPACING);
	pulse();//play
	delayLongAudio(FINAL_SUCCESS_LENGTH);
	pulse();//pause at end of track
	
	lastTrack = FinalSuccess;//set last track to this track
	delayLongAudio(EVENT_SPACING);
}

#ifdef TEST 
/* test Harness for testing this module */ 
#include "termio.h" 
int main(void) 
{ 
	_HW_Timer_Init(ES_Timer_RATE_1mS);
	TERMIO_Init();
	clrScrn();
	printf("In Test Harness for Audio Control \n\r");
	AudioInit();
	
	PlayWoohoo();
	PlayTimesUp();
	PlayFinalSuccess();
	PlayMamaMia();
	PlayWoohoo();
	
	return 0; 
} 
#endif 
