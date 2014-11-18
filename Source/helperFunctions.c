#include "helperFunctions.h"

void wait (int delay) { //check variable type
	unsigned int time_start = ES_Timer_GetTime(); 
	unsigned int time = ES_Timer_GetTime();
	
	while (time < time_start + delay) {
		HWREG(SYSCTL_RCGCGPIO);
		time = ES_Timer_GetTime();
	}
}
