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
 * 2 - Mama Mia
 * 3 - Time's Up
 * 4 - Very Good Finish
 */
void Pause(void);
void PlayWohoo(void);
void PlaySuccessFinish(void);
void PlayMamaMia(void);
void PlayGameOver(void);

#endif 

