#ifndef LEDS_H
#define LEDS_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_sysctl.h"
#include "driverlib/sysctl.h"
#include "ES_Port.h"
#include "ES_Timers.h"
#include "EnablePA25_PB23_PD7_PF0.h"
#include "termio.h"
#include "helperFunctions.h"

// not as sure if these are absolutley needed
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "utils/uartstdio.h"

void LEDShiftRegInit (void);
void setLED (char LEDs[8]);

#endif
