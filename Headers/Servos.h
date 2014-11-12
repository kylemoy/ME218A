#ifndef KEYHOLE_H
#define KEYHOLE_H
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

double PotRead (void);
void initializeServos(void);
void rotateServo(uint8_t channel, uint16_t position);
void rotateServoLeft(uint8_t channel);
void rotateServoRight(uint8_t channel);
void rotateServoToPlusNinety(uint8_t channel);
void rotateServoToMinusNinety(uint8_t channel);
void rotateServoToZero(uint8_t channel);

#endif
