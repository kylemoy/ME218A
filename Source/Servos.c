//#define TEST
/* Test the Servo module */

#include "Servos.h" 

#define POT_CHAN 8 // 11=PB5, 3 = PE0, 7 = PD0, 8 = PE5
#define POT_HIGH 3.3 // High on Pot
#define POT_LOW 0 // Low on Pot
#define TOWER_HIGH 2050
#define TOWER_LOW 1650


#define KEY_SERVO1 0 // Pin PB6
#define KEY_SERVO2 1 // Pin PB7
#define TOWER_SERVO 2 // Pin PB4

#define PLUS_90_DEGREES		2850
#define ZERO_DEGREES 			1750
#define MINUS_90_DEGREES 	750

static uint16_t key_servo_pos = ZERO_DEGREES;
static uint16_t tower_servo_pos = ZERO_DEGREES;

double getPotValue(void) {
	static double conversion = POT_HIGH / 4096; //Volts/point value
	double Direct_pot = 0; //this variable will be used to store the direct readout value
	double pot_val = 0; //the pot value converted to volts
	Direct_pot = (double) ADC0_InSeq3(); // reads pin value, cast as a double
	pot_val = Direct_pot * conversion;
	return pot_val;
}

void initializeServos(void) {
	ADC0_InitSWTriggerSeq3(POT_CHAN); // Initializes the pot
	PWM_TIVA_Init();
  PWM_TIVA_SetFreq(50, 0);
  PWM_TIVA_SetFreq(50, 1);
  PWM_TIVA_SetPulseWidth(key_servo_pos, KEY_SERVO1);
	PWM_TIVA_SetPulseWidth(key_servo_pos, KEY_SERVO2);
	PWM_TIVA_SetPulseWidth(tower_servo_pos,TOWER_SERVO);
}

void rotateServo(uint8_t channel, uint16_t position) {
	if (channel == KEY_SERVO1 || channel == KEY_SERVO2) {
		if (!(position < MINUS_90_DEGREES || position > PLUS_90_DEGREES))
			key_servo_pos = position;
		PWM_TIVA_SetPulseWidth(key_servo_pos, KEY_SERVO1);
		PWM_TIVA_SetPulseWidth(key_servo_pos, KEY_SERVO2);
	} else {
		if (!(position < MINUS_90_DEGREES || position > PLUS_90_DEGREES))
			tower_servo_pos = position;
		PWM_TIVA_SetPulseWidth(tower_servo_pos,TOWER_SERVO);
	}
}

void rotateServoLeft(uint8_t channel) {
	if (channel == KEY_SERVO1 || channel == KEY_SERVO2) {
		if (!(key_servo_pos - 10 < MINUS_90_DEGREES))
			key_servo_pos = key_servo_pos - 10;
		PWM_TIVA_SetPulseWidth(key_servo_pos, KEY_SERVO1);
		PWM_TIVA_SetPulseWidth(key_servo_pos, KEY_SERVO2);
	} else {
		if (!(tower_servo_pos - 10 < MINUS_90_DEGREES))
			tower_servo_pos = tower_servo_pos - 10;
		PWM_TIVA_SetPulseWidth(tower_servo_pos,TOWER_SERVO);
	}
}

void rotateServoRight(uint8_t channel) {
	if (channel == KEY_SERVO1 || channel == KEY_SERVO2) {
		if (!(key_servo_pos + 10 > PLUS_90_DEGREES))
			key_servo_pos = key_servo_pos + 10;
		PWM_TIVA_SetPulseWidth(key_servo_pos, KEY_SERVO1);
		PWM_TIVA_SetPulseWidth(key_servo_pos, KEY_SERVO2);
	} else {
		if (!(tower_servo_pos + 10 > PLUS_90_DEGREES))
			tower_servo_pos = tower_servo_pos + 10;
		PWM_TIVA_SetPulseWidth(tower_servo_pos,TOWER_SERVO);
	}
}

void rotateServoToPlusNinety(uint8_t channel) {
	rotateServo(channel, PLUS_90_DEGREES);
}

void rotateServoToMinusNinety(uint8_t channel) {
	rotateServo(channel, MINUS_90_DEGREES);
}

void rotateServoToZero(uint8_t channel) {
	rotateServo(channel, ZERO_DEGREES);
}


#ifdef TEST
/* Test Harness for testing the LCD module */

int main(void)
{
	// Set the clock to run at 40MhZ using the PLL and 16MHz external crystal
	SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN
			| SYSCTL_XTAL_16MHZ);
  // initialize the timer sub-system and console I/O
  _HW_Timer_Init(ES_Timer_RATE_1mS);
	TERMIO_Init();
	clrScrn();

	puts("\rStarting Servo Test \r");
	printf("%s %s\n",__TIME__, __DATE__);
	printf("\n\r\n");

  initializeServos();
	while (true) {
		//double potValue = getPotValue();
		//uint16_t tower_servo_pos = (potValue - POT_LOW) / (POT_HIGH - POT_LOW) * (TOWER_HIGH - TOWER_LOW) + TOWER_LOW;\
		//printf("\n\r Pot val: %lf, Tower servo pos: %d \r\n", potValue, tower_servo_pos);
		//rotateServo(KEY_SERVO1, tower_servo_pos);
		char input = getchar();
		switch (input) {
			case 'a':
				rotateServoLeft(KEY_SERVO1);
				printf("Servo Position is %d\r\n", key_servo_pos);
				break;
			
			case 's':
				rotateServoRight(KEY_SERVO1);
				printf("Servo Position is %d\r\n", key_servo_pos);
				break;
			
			case 'q':
				rotateServoToMinusNinety(KEY_SERVO1);
				printf("Rotating servo to -90 degrees...\r\n");
				printf("Servo Position is %d\r\n", key_servo_pos);
				break;
			
			case 'w':
				rotateServoToZero(KEY_SERVO1);
				printf("Rotating servo to 0 degrees...\r\n");
				printf("Servo Position is %d\r\n", key_servo_pos);
				break;
			
			case 'e':
				rotateServoToPlusNinety(KEY_SERVO1);
				printf("Rotating servo to +90 degrees...\r\n");
				printf("Servo Position is %d\r\n", key_servo_pos);
				break;
		}
	}
}
#endif
