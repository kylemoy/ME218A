#ifndef AudioControl_H 
#define AudioControl_H 

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
 * IMPORTANT; MP3 MUST BEGIN AT 'Wohoo' TRACK!
 */
void AudioInit(void);

/* 
 * Functions to play and pause specific tracks 
 * 
 * IMPORTANT: Order of Tracks
 * 1 - Woohoo
 * 2 - Woohoo
 * 3 - Very Good Finish
 */

void PlayWoohoo(void);
void PlayFinalSuccess(void);

#endif 

