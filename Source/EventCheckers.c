/****************************************************************************
 Module
   EventCheckers.c

 Revision
   1.0.1 

 Description
   This is the sample for writing event checkers along with the event 
   checkers used in the basic framework test harness.

 Notes
   Note the use of static variables in sample event checker to detect
   ONLY transitions.
   
 History
 When           Who     What/Why
 -------------- ---     --------
 08/06/13 13:36 jec     initial version
****************************************************************************/

// this will pull in the symbolic definitions for events, which we will want
// to post in response to detecting events
#include "ES_Configure.h"
// this will get us the structure definition for events, which we will need
// in order to post events in response to detecting events
#include "ES_Events.h"
// if you want to use distribution lists then you need those function 
// definitions too.
#include "ES_PostList.h"
// This include will pull in all of the headers from the service modules
// providing the prototypes for all of the post functions
#include "ES_ServiceHeaders.h"
// this test harness for the framework references the serial routines that
// are defined in ES_Port.c
#include "ES_Port.h"
// include our own prototypes to insure consistency between header & 
// actual functionsdefinition
#include "EventCheckers.h"
#include "TapeSensor.h"
#include "Servos.h"
#include "SlotDetector.h"



/****************************************************************************
 Function
   Check4Keystroke
 Parameters
   None
 Returns
   bool: true if a new key was detected & posted
 Description
   checks to see if a new key from the keyboard is detected and, if so, 
   retrieves the key and posts an ES_NewKey event to TestHarnessService0
 Notes
   The functions that actually check the serial hardware for characters
   and retrieve them are assumed to be in ES_Port.c
   Since we always retrieve the keystroke when we detect it, thus clearing the
   hardware flag that indicates that a new key is ready this event checker 
   will only generate events on the arrival of new characters, even though we
   do not internally keep track of the last keystroke that we retrieved.
 Author
   J. Edward Carryer, 08/06/13, 13:48
****************************************************************************/


bool Check4Keystroke(void)
{
  if ( IsNewKeyReady() )
  {
    ES_Event ThisEvent;
    ThisEvent.EventType = ES_NEW_KEY;
    ThisEvent.EventParam = GetNewKey();
		if ( ThisEvent.EventParam == '1'){
			ThisEvent.EventType = THREE_HANDS_ON;
      PostDisarmFSM( ThisEvent );
		} else if ( ThisEvent.EventParam == '2'){
			ThisEvent.EventType = THREE_HANDS_OFF;
      PostDisarmFSM( ThisEvent );
		} else if ( ThisEvent.EventParam == '3'){
			ThisEvent.EventType = CORRECT_PASSWORD_ENTERED;
      PostDisarmFSM( ThisEvent );
		} else if ( ThisEvent.EventParam == '4'){
			ThisEvent.EventType = INCORRECT_PASSWORD_ENTERED;
      PostDisarmFSM( ThisEvent );
		} else if ( ThisEvent.EventParam == '5'){
			ThisEvent.EventType = KEY_INSERTED ;
      PostDisarmFSM( ThisEvent );
		} else if ( ThisEvent.EventParam == '6'){
			ThisEvent.EventType = CORRECT_VALUE_DIALED ;
      PostDisarmFSM( ThisEvent );
		} else   {
			PostDisarmFSM( ThisEvent );
		}
    return true;
  }
  return false;
}


bool CheckTapeSensor(void) {
	static bool lastState = true;
	if (tapeSensorsCovered() && (lastState == false)) {
		ES_Event ThisEvent;
    ThisEvent.EventType = THREE_HANDS_ON ;
    PostDisarmFSM( ThisEvent );
		lastState = true;
		return true;
	} else if (!(tapeSensorsCovered()) && (lastState == true)) {
		ES_Event ThisEvent;
    ThisEvent.EventType = THREE_HANDS_OFF ;
    PostDisarmFSM( ThisEvent );
		lastState = false;
		return true;
	}
	return false;
}

bool CheckSlotDetector(void) {
	int keyInSlotDefault = getKeyInSlotDefault();
	if (keyInSlotDefault == -1) {
		printf("Setting key in slot default.\r\n");
		setKeyInSlotDefault();
	}

	static bool lastState = false;
	bool keyInSlot = isKeyInSlot();

	if (keyInSlot && (lastState == false)) {
		printf("Key has been placed.\r\n");
		lastState = true;
		ES_Event ThisEvent;
    ThisEvent.EventType = KEY_INSERTED ;
    PostDisarmFSM( ThisEvent );
		return true;

	} else if (!keyInSlot && (lastState == true)) {
		printf("Key has been removed.\r\n");
		ES_Event ThisEvent;
    ThisEvent.EventType = KEY_INSERTED ;
    PostDisarmFSM( ThisEvent );
		lastState = false;
		return true;
	}
	return false;
//	bool lastState = false;
//	bool keyRemoved = isKeyRemoved();

//	if (keyRemoved && (lastState == false)) {
//		printf("Key has been removed.");
//		lastState = true;
//		return true;

//	} else if (!keyRemoved && (lastState == true)) {
//		printf("Key has been placed.");
//		ES_Event ThisEvent;
//    ThisEvent.EventType = KEY_INSERTED ;
//    PostDisarmFSM( ThisEvent );
//		lastState = false;
//		return true;
//	}
//	return false;
//
}

bool CheckPot(void) {
	static double lastPotValue = -1;
	double potZero = getPotZero();
	double potValue = getPotValue();
	bool victory = getVictory();
	// If pot has not been initialized yet or if victory has been achieved, do nothing
	if (potZero == -1 || victory) {
		return false;
		// Check victory condition for Game Type A
	} else if (potZero > 1.5 && (potValue < (potZero - 1.45)) && (potValue > (potZero - 1.55))) {
		printf("Pot Value: %f\r\n", potValue);
		printf("Correct Value!\r\n");
		ES_Event ThisEvent;
    ThisEvent.EventType = CORRECT_VALUE_DIALED ;
    PostDisarmFSM( ThisEvent );
		setVictory(true);
		return true;
		// Check victory condition for Game Type B
	} else if (potZero < 1.5 && (potValue < (potZero + 1.55)) && (potValue > (potZero + 1.45))) {
		printf("Pot Value: %f\r\n", potValue);
		printf("Correct Value!\r\n");
		ES_Event ThisEvent;
    ThisEvent.EventType = CORRECT_VALUE_DIALED ;
    PostDisarmFSM( ThisEvent );
		setVictory(true);
		return true;
	// Check for pot changes
	} else if (potValue < (lastPotValue - 0.05) || potValue > (lastPotValue + 0.05)) {
		moveTower(potValue);
		lastPotValue = potValue;
	}
	return false;
	
}

