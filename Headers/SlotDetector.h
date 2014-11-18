#ifndef SLOTDETECTOR_H
#define SLOTDETECTOR_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_sysctl.h"
#include "driverlib/sysctl.h"
#include "termio.h"
#include "ES_Port.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "utils/uartstdio.h"

void setKeyInSlotDefault(void);
int getKeyInSlotDefault(void);
void initPhototransistor(void);
bool isKeyInSlot(void);

#endif
