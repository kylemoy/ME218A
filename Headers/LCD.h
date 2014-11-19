#ifndef _LCD_
#define _LCD_
#include <stdint.h>
#include "helperFunctions.h"

void resetLCDmessage(void);
void printLCDmessage(void);
void sendToLCD (char Data); // takes 8 bit input and pulses E to input into LCD
void sendToShiftReg (char Data); // sends the 8 bit input to shift register
void LCDInit (void); //initializes LP and then 4 bit mode of LCD
void LCDputchar (char letter);
void clearLCD(void);
void printAuthorizedMessage(void);
void printIncorrectMessage(void);
void printArmedMessage(void);
void printTimeUp(void);
#endif
