#include "LEDs.h"
#include "TapeSensor.h"
#include "Servos.h"

int main(void) { 
	// Set the clock to run at 40MhZ using the PLL and 16MHz external crystal
	SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN
			| SYSCTL_XTAL_16MHZ);
  // initialize the timer sub-system and console I/O
  _HW_Timer_Init(ES_Timer_RATE_1mS);
	TERMIO_Init(); 
	printf("\n\r Testing for First Checkoff \r\n");
	printf("\n\r 1) LED, 2) Tape Sensors, 3) Servos \r\n");
	initTapeSensors();
	while(true){
		char input = getchar();
			switch (input) {
			case '1':
				printf("\n\r Testing for the LEDs \r\n");
				char test[8] =  { 0,1};
				ShiftRegInit();
				ShiftRegLED (test);
				break;
			
			case '2':
				printf("\n\rIn Testing the Tape Sensors\r\n");
				
				while(true){
					if (tapeSensorsCovered()) {
						printf("All three tape sensors are covered.\r\n");
					} else {
						printf("One or more tape sensors are not covered.\r\n");
					}
				}
				break;
			
			case '3':
				printf("\n\rIn Testing the Servos\r\n");
				initializeServos();
				while (true) {
					double potValue = getPotValue();
					uint16_t tower_servo_pos = (potValue - POT_LOW) / (POT_HIGH - POT_LOW) * (TOWER_HIGH - TOWER_LOW) + TOWER_LOW;
					printf("\n\r Pot val: %lf, Tower servo pos: %d \r\n", potValue, tower_servo_pos);
					rotateServo(KEY_SERVO1, tower_servo_pos);
				}
				break;
			}
	}
}
