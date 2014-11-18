/****************************************************************************
 
  Header file for KeyPad Sate Machine 
  based on the Gen2 Events and Services Framework

 ****************************************************************************/

#ifndef KEYPAD_FSM_H
#define KEYPAD_FSM_H

// Event Definitions
#include "ES_Configure.h" /* gets us event definitions */
#include "ES_Types.h"     /* gets bool type for returns */

// typedefs for the states
// State definitions for use with the query function
 typedef enum { InitPState, ButtonPressed } KeyPadFSMState_t ;


// Public Function Prototypes

bool CheckForKeyPadButtonPress( void );
bool InitKeyPadFSM ( uint8_t Priority );
bool PostKeyPadFSM( ES_Event ThisEvent );
ES_Event RunKeyPadFSM( ES_Event ThisEvent );
KeyPadFSMState_t QueryKeyPadFSM ( void );


#endif /* KEYPAD_FSM_H */

