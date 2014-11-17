#ifndef SERVOS_H
#define SERVOS_H
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "ES_Port.h"
#include "inc/hw_memmap.h" 
#include "inc/hw_types.h" 
#include "inc/hw_gpio.h" 
#include "inc/hw_sysctl.h" 
#include "driverlib/gpio.h" 
#include "driverlib/sysctl.h"
#include "termio.h"
#include "PWMTiva.h"
#include "ADCSWTrigger.h"

#define clrScrn() 	printf("\x1b[2J")

// Public functions
void raiseFlag(void);
void lowerFlag(void);
void lockKeys(void);
void unlockKeys(void);
void setVictory(bool state);
bool getVictory(void);
void setPotZero(void);
double getPotZero(void);
double getPotValue(void);
void initializeServos(void);
void setTowerToZero(void);
void moveTower(double potValue);

// Private functions
void rotateServo(uint8_t channel, uint16_t position);
void rotateServoLeft(uint8_t channel);
void rotateServoRight(uint8_t channel);
void rotateServoToPlusNinety(uint8_t channel);
void rotateServoToMinusNinety(uint8_t channel);
void rotateServoToZero(uint8_t channel);

#endif
