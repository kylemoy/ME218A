/****************************************************************************
 
  Header file for the background service (to handle response to timers and keystrokes)
  based on the Gen 2 Events and Services Framework

 ****************************************************************************/

#ifndef BackgroundService_H
#define BackgroundService_H

#include "ES_Types.h"

// Public Function Prototypes

bool InitBackgroundService ( uint8_t Priority );
bool PostBackgroundService( ES_Event ThisEvent );
ES_Event RunBackgroundService( ES_Event ThisEvent );


#endif /* BackgroundService_H */

