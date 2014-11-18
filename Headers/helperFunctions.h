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
#include "termio.h"
#include "driverlib/gpio.h"

void wait (int delay); // a wait function
