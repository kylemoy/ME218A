/****************************************************************************
 Module
     EventCheckers.h
 Description
     header file for the event checking functions
 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 08/06/13 14:37 jec      started coding
*****************************************************************************/

#ifndef EventCheckers_H
#define EventCheckers_H
#include "KeyPadFSM.h"

// prototypes for event checkers
bool CheckForKeyPadButtonPress( void );
bool Check4Keystroke(void);
bool CheckTapeSensor(void);
bool CheckPot(void);


#endif /* EventCheckers_H */
