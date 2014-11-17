/****************************************************************************
 Module
   DisarmFSM.c

 Revision
   1.0.1

 Description
   This is a template file for implementing flat state machines under the
   Gen2 Events and Services Framework.

 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 01/15/12 11:12 jec      revisions for Gen2 framework
 11/07/11 11:26 jec      made the queue static
 10/30/11 17:59 jec      fixed references to CurrentEvent in RunTemplateSM()
 10/23/11 18:20 jec      began conversion from SMTemplate.c (02/20/07 rev)
****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "DisarmFSM.h"
#include "TapeSensor.h"
#include "DCmotors.h"
#include "LEDs.h"
#include "Servos.h"

/*----------------------------- Module Defines ----------------------------*/
#define ON 1
#define OFF 0

//LEDS
#define Tier1 1
#define Tier2 2
#define Tier3 3
#define Tier4 4
#define Tier5 5
#define Tier6 6
#define pot 7


/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine.They should be functions
   relevant to the behavior of this state machine
*/

/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, you may need others as well.
// type of state variable should match htat of enum in header file
static DisarmState_t CurrentState;

// with the introduction of Gen2, we need a module level Priority var as well
static uint8_t MyPriority;


/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     DisarmTemplateFSM

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
bool InitDisarmFSM ( uint8_t Priority )
{
  ES_Event ThisEvent;
	
	// Initialize the Tape Sensors
	initTapeSensors();
	initMotors();
	initializeServos();
	LEDShiftRegInit();
	
  MyPriority = Priority;
  CurrentState = Armed;
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
     PostDisarmFSM

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
bool PostDisarmFSM( ES_Event ThisEvent )
{
  return ES_PostToService( MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunDisarmFSM

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
ES_Event RunDisarmFSM( ES_Event ThisEvent )
{
  ES_Event ReturnEvent;
  ReturnEvent.EventType = ES_NO_EVENT; // assume no errors
	
	char LEDs[8] = {0,0,0,0,0,0,0,0};
	
  switch ( CurrentState )
  {
    case Armed :
			switch ( ThisEvent.EventType ) {
				case ES_INIT :
					printf("Arming...\r\n");
				
					printf(" Turning vibration motor on...\r\n");
					vibrationMotorOn(); // turns vibration motor ON
					
					printf(" Setting all tower LEDS off...\r\n");
					setLED (LEDs);
					printf(" Lowering flag...\r\n");
					lowerFlag();
					printf(" Locking the keys\r\n");
					lockKeys();
					printf(" Raising ball and feather...\r\n");
					unwindTimingMotor();
					printf(" Setting tower to 0...\r\n\r\n");
					setTowerToZero();
					printf("STATE: Armed\r\n\r\n");
					
					break;
        
				case THREE_HANDS_ON :
					printf("EVENT: Three hands detected.\r\n");
				
					printf(" Turning  vibration motor off...\r\n");
					vibrationMotorOff();
				
					printf(" Setting Tower Tier 1 LED on...\r\n");
					LEDs[Tier1] = 1;
					setLED(LEDs);
				
					printf(" Begin printing LCD passcode...\r\n");
					//printLCDmessage();
					printf(" Starting 60s disarm timer...\r\n");
					
					//the timer can only go up to 32000, so we need to set the new timer rate to 2ms
					ES_Timer_Init(ES_Timer_RATE_2mS);
					ES_Timer_InitTimer(DISARM_TIMER, 30000);
					ES_Timer_StartTimer(DISARM_TIMER);
					
					printf(" Transitioning to Stage 1...\r\n\r\n");
					CurrentState = Stage1;
					printf("STATE: Stage1\r\n\r\n");
					break;
				
				default:
					;
			}
			break;

    case Stage1 :
      switch ( ThisEvent.EventType )
      {
				case ES_TIMEOUT :
					if (ThisEvent.EventParam == DISARM_TIMER) {
						printf("EVENT: Time has run out!\r\n");
						ThisEvent.EventType = ES_INIT;
						PostDisarmFSM(ThisEvent);
						CurrentState = Armed;
					}
					
					if (ThisEvent.EventParam == VIBRATION_TIMER) {
						printf(" Turnnign off vibration pulse...\r\n\r\n");
						vibrationMotorOff();
					}
          break;
				case THREE_HANDS_OFF :
					printf("EVENT: One or more hands have been released.\r\n");
					printf(" Clearing the LCD screen\r\n");
					//clearLCD();
					printf(" Setting Tower Tier 1 LED off...\r\n");
					LEDs[Tier1] = 0;
				  setLED(LEDs);
				
					printf(" Transitioning to Stage1_Stagnated...\r\n\r\n");
					CurrentState = Stage1_Stagnated;
					printf("STATE: Stage1_Stagnated\r\n\r\n");
          break;
				
				case CORRECT_PASSWORD_ENTERED :
					printf("EVENT: The correct password has been entered.\r\n");
					printf(" Unlocking the keys\r\n");
					unlockKeys();
				
					printf(" Setting Tower Tier 2 LED on...\r\n");
					LEDs[Tier2] = 1;
				  setLED(LEDs);
				
					printf(" Playing audio: Wahoo!...\r\n");
				  //playWahoo();
					printf(" Transitioning to Stage2...\r\n\r\n");
					CurrentState = Stage2;
					printf("STATE: Stage2\r\n\r\n");
          break;
				
				case INCORRECT_PASSWORD_ENTERED :
					printf("EVENT: The incorrect password has been entered.\r\n");
					printf(" Generating vibration pulse...\r\n\r\n");
					vibrationMotorOn();
					ES_Timer_InitTimer(VIBRATION_TIMER, 350);
					ES_Timer_StartTimer(VIBRATION_TIMER);
          break;

        default :
            ; 
      }
      break;
		
		case Stage1_Stagnated :
      switch ( ThisEvent.EventType )
      {
        case ES_TIMEOUT :
          if (ThisEvent.EventParam == DISARM_TIMER) {
						printf("EVENT: Time has run out!\r\n");
						ThisEvent.EventType = ES_INIT;
						PostDisarmFSM(ThisEvent);
						CurrentState = Armed;
					}
          break;
				
				case THREE_HANDS_ON :
					printf("EVENT: Three hands detected.\r\n");
				
					printf(" Setting Tower Tier 1 LED on...\r\n");
					LEDs[Tier1] = 1;
					setLED(LEDs);
					
					printf(" Begin printing LCD passcode...\r\n");
					//printLCDmessage();
					printf(" Transitioning to Stage1...\r\n\r\n");
					CurrentState = Stage1;
					printf("STATE: Stage1\r\n\r\n");
          break;

        default :
            ; 
      }
      break;
			
		case Stage2 :
      switch ( ThisEvent.EventType )
      {
        case ES_TIMEOUT :
          if (ThisEvent.EventParam == DISARM_TIMER) {
						printf("EVENT: Time has run out!\r\n");
						ThisEvent.EventType = ES_INIT;
						PostDisarmFSM(ThisEvent);
						CurrentState = Armed;
          }
					break;
					
				case KEY_INSERTED:
					printf("EVENT: Key has been inserted.\r\n");
				
					printf(" Setting Tower Tier 3 LED on...\r\n");
					LEDs[Tier3] = 1;
					setLED(LEDs);
				
					printf(" Setting Dial LED on...\r\n");
					LEDs[pot] = 1;
					setLED(LEDs);
				
					printf(" Playing audio: Wahoo!...\r\n");
					//playWahoo();
					printf("Initializing the pot value...\r\n");
					setPotZero();
					printf(" Transitioning to Stage3...\r\n\r\n");
					CurrentState = Stage3;
					printf("STATE: Stage3\r\n\r\n");
          break;

        default :
            ; 
      }
      break;			

		case Stage3 :
      switch ( ThisEvent.EventType )
      {
        case ES_TIMEOUT :
					if (ThisEvent.EventParam == DISARM_TIMER) {  
						printf("EVENT: Time has run out!\r\n");
						ThisEvent.EventType = ES_INIT;
						PostDisarmFSM(ThisEvent);
						CurrentState = Armed;
					}
					if (ThisEvent.EventParam == FAST_LEDS) {  
						printf("EVENT: \r\n");
						ThisEvent.EventType = CORRECT_VALUE_DIALED;
						PostDisarmFSM(ThisEvent);
					}
          break;
				
				case CORRECT_VALUE_DIALED :
					printf("EVENT: The correct pot value has been dialed.\r\n");
					
					printf(" Setting Tower Tier 4-6 LED on with delay...\r\n");
					
					ES_Timer_InitTimer(FAST_LEDS, 150);
					static int i = Tier4;
					if (i<=Tier6){
						printf("\n\r looping for LED i + %d\n\r", i);
						LEDs[i] = 1;
						setLED(LEDs);
						ES_Timer_StartTimer(FAST_LEDS);
						i++;
						break;
					}
					printf(" Raising the flag...\r\n");
					raiseFlag();
					printf(" Playing audio: victory song...\r\n");
					//playVictorySong();
					printf(" Starting 30s post-disarm timer...\r\n");
					ES_Timer_InitTimer(POST_DISARM_TIMER, 30000);
					printf(" Transitioning to Stage4...\r\n\r\n");
					CurrentState = Stage4;
					printf("STATE: Stage4\r\n\r\n");
          break;

        default :
            ; 
      }
      break;	

		case Stage4 :
      switch ( ThisEvent.EventType )
      {
        case ES_TIMEOUT :
					if (ThisEvent.EventParam == POST_DISARM_TIMER) {
						printf("EVENT: Post-disarm timer expired.\r\n");
						ThisEvent.EventType = ES_INIT;
						PostDisarmFSM(ThisEvent);
						CurrentState = Armed;
					}
          break;

        default :
            ; 
      }
      break;	

    default :
      ;
  }
  return ReturnEvent;
}

/****************************************************************************
 Function
     QueryDisarmFSM

 Parameters
     None

 Returns
     DisarmState_t The current state of the Disarm state machine

 Description
     returns the current state of the Disarm state machine
 Notes

 Author
     J. Edward Carryer, 10/23/11, 19:21
****************************************************************************/
DisarmState_t QueryDisarmFSM ( void )
{
   return(CurrentState);
}

/***************************************************************************
 private functions
 ***************************************************************************/

