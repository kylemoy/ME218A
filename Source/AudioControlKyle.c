/****************************************************************************
 Module
   AudioControl.c
****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
#include "ES_Configure.h"
#include "ES_Framework.h"
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
#include "AudioControlKyle.h"

/*----------------------------- Module Defines ----------------------------*/
#define ALL_BITS (0xFF<<2)
#define AUDIO_PIN (GPIO_PIN_2)
#define PULSE_WIDTH 300 //in milliseconds
#define MULT_PULSE_SPACING 300 //in milliseconds
#define EVENT_SPACING 200
#define NUM_TRACKS 4

// Track Lengths in milliseconds
#define WOOHOO_LENGTH 180
#define MAMA_MIA_LENGTH 750
#define FINAL_SUCCESS_LENGTH 1200
#define TIMES_UP_LENGTH 2000

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this service.They should be functions
   relevant to the behavior of this service
*/

/*---------------------------- Module Variables ---------------------------*/
// with the introduction of Gen2, we need a module level Priority variable
static uint8_t MyPriority;

void initAudio(void) {
	  // Initialize port lines for Audio
  PortFunctionInit(); // set PA2-5, PB2-3, PD7, PF0 to GPIO
  // wait a few clock cycles
  uint8_t pause = HWREG(SYSCTL_RCGCGPIO); 
  /* Enable Ports on the Tiva
   * Port F | Port E | Port D | Port C | Port B | Port A
   * Bit 5  | Bit 4  | Bit 3  | Bit 2  | Bit 1  | Bit 0 
   * example: HWREG(SYSCTL_RCGCGPIO) |= BIT1HI; // Enable GPIO Port B
   */
  HWREG(SYSCTL_RCGCGPIO) |= BIT5HI; // Enable GPIO Port F
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


/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitTemplateService

 Parameters
     uint8_t : the priorty of this service

 Returns
     bool, false if error in initialization, true otherwise

 Description
     Saves away the priority, and does any 
     other required initialization for this service
 Notes

 Author
     J. Edward Carryer, 01/16/12, 10:00
****************************************************************************/
bool InitAudioControlService ( uint8_t Priority )
{
  ES_Event ThisEvent;

  MyPriority = Priority;
 initAudio();
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
     PostTemplateService

 Parameters
     EF_Event ThisEvent ,the event to post to the queue

 Returns
     bool false if the Enqueue operation failed, true otherwise

 Description
     Posts an event to this state machine's queue
 Notes

 Author
     J. Edward Carryer, 10/23/11, 19:25
****************************************************************************/
bool PostAudioControlService( ES_Event ThisEvent )
{
  return ES_PostToService( MyPriority, ThisEvent);
}

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

/****************************************************************************
 Function
    RunTemplateService

 Parameters
   ES_Event : the event to process

 Returns
   ES_Event, ES_NO_EVENT if no error ES_ERROR otherwise

 Description
   add your description here
 Notes
   
 Author
   J. Edward Carryer, 01/15/12, 15:23
****************************************************************************/
ES_Event RunAudioControlService( ES_Event ThisEvent )
{
  ES_Event ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors
  switch (ThisEvent.EventType) {
    case ES_INIT :
      break;

    case ES_TIMEOUT :
			if ( ThisEvent.EventParam == SOUND_TIMER){
				printf("Sound timer1 expired!\r\n");
				pulse();
				//moveForward();
				//ES_Timer_Init(ES_Timer_RATE_1mS);
				//ES_Timer_InitTimer(SOUND_TIMER2, EVENT_SPACING);
				//ES_Timer_StartTimer(SOUND_TIMER2);
			} else if ( ThisEvent.EventParam == SOUND_TIMER2) {
				printf("Sound timer2 expired!\r\n");
				moveForward();
				ES_Timer_Init(ES_Timer_RATE_1mS);
				ES_Timer_InitTimer(SOUND_TIMER3, EVENT_SPACING);
				ES_Timer_StartTimer(SOUND_TIMER3);
			} else if ( ThisEvent.EventParam == SOUND_TIMER3) {
				printf("Sound timer3 expired!\r\n");
				pulse();
			}
      break;
		

    case PLAY_WAHOO1 :
			initAudio();
      pulse();//play
      ES_Timer_Init(ES_Timer_RATE_1mS);
      ES_Timer_InitTimer(SOUND_TIMER, 415);
      ES_Timer_StartTimer(SOUND_TIMER);
      break;

    case PLAY_WAHOO2 :
			pulse();//play
      ES_Timer_Init(ES_Timer_RATE_1mS);
      ES_Timer_InitTimer(SOUND_TIMER, 415);
      ES_Timer_StartTimer(SOUND_TIMER);
      break;

    case PLAY_FINAL_SUCCESS:
      pulse();//play
      ES_Timer_Init(ES_Timer_RATE_1mS);
      ES_Timer_InitTimer(SOUND_TIMER2, 3200);
      ES_Timer_StartTimer(SOUND_TIMER2);
      break;


  }
  return ReturnEvent;
}

/***************************************************************************
 private functions
 ***************************************************************************/

/*------------------------------- Footnotes -------------------------------*/
/*------------------------------ End of file ------------------------------*/

