
#ifndef AudioControl_H
#define AudioControl_H

#include "ES_Types.h"

// Public Function Prototypes

void initAudio(void) ;
bool InitAudioControlService ( uint8_t Priority );
bool PostAudioControlService( ES_Event ThisEvent );
ES_Event RunAudioControlService( ES_Event ThisEvent );


#endif /* AudioControl_H */

