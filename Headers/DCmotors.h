#ifndef DCMOTORS_H
#define DCMOTORS_H

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

void initMotors(void);
void rewindTimingMotor(void);
void unwindTimingMotor(void);
void stopTimingMotor(void);
void vibrationMotorOn(void);
void vibrationMotorOff(void);
void setMotor(int motor, int state);
bool getMotorState(int motor);

#endif
