//#define TEST
/* Test the Servo module */

#include "Servos.h" 

#define POT_CHAN 8 // 11=PB5, 3 = PE0, 7 = PD0, 8 = PE5
#define POT_HIGH 3.3 // High on Pot
#define POT_LOW 0 // Low on Pot

#define TOWER_HIGH 1500
#define TOWER_ZERO_DEGREES 			1400
#define TOWER_LOW 1350

#define KEY_LOCK	920
#define KEY_UNLOCK	2020

#define FLAG_RAISED 1175
#define FLAG_LOWERED 2250


#define TOWER_SERVO 3 // Pin PB5
#define TOWER_ROTATE_SERVO 1 // Pin PB7
#define KEY_SERVO 2 // Pin PB4
#define FLAG_SERVO 0 // Pin PB6

#define PLUS_90_DEGREES		2850
#define ZERO_DEGREES 			1400
#define MINUS_90_DEGREES 	750
 
static double potZero = -1;
static bool victory = false;
static uint16_t key_servo_pos = ZERO_DEGREES;
static uint16_t tower_servo_pos = ZERO_DEGREES;
static uint16_t tower_rotate_servo_pos = ZERO_DEGREES;
static uint16_t flag_servo_pos = ZERO_DEGREES;


void raiseFlag(void) {
	rotateServo(FLAG_SERVO, FLAG_RAISED);
}

void lowerFlag(void) {
	rotateServo(FLAG_SERVO, FLAG_LOWERED);
}

void lockKeys(void) {
	rotateServo(KEY_SERVO, KEY_LOCK);
}

void unlockKeys(void) {
	rotateServo(KEY_SERVO, KEY_UNLOCK);
}


void setVictory(bool state) {
	victory = state;
}

bool getVictory(void) {
	return victory;
}

void setPotZero(void) {
	potZero = getPotValue();
	victory = false;
}
double getPotZero(void) {
	return potZero;
}

double getPotValue(void) {
	static double conversion = POT_HIGH / 4096; //Volts/point value
	double Direct_pot = 0; //this variable will be used to store the direct readout value
	double pot_val = 0; //the pot value converted to volts
	Direct_pot = (double) ADC0_InSeq3(); // reads pin value, cast as a double
	pot_val = Direct_pot * conversion;
	return pot_val;
}

void moveTower(double potValue) {
	if (potZero == -1) {
		printf("Error: PotZero not inititialized.");
		return;
	}
	// High on Pot = Low on Motor
	if (potZero > 1.5) {
		printf("\n\r Game Type A\r\n");
		uint16_t tower_servo_pos = (potZero - potValue) / (potZero - POT_LOW) * (TOWER_ZERO_DEGREES - TOWER_LOW) + TOWER_ZERO_DEGREES;
		printf("\n\r Pot val: %lf, Tower servo pos: %d \r\n", potValue, tower_servo_pos);
		rotateServo(TOWER_SERVO, tower_servo_pos);
	// Low on Pot = High on Motor
	} else {
		printf("\n\r Game Type B\r\n");
		uint16_t tower_servo_pos = TOWER_ZERO_DEGREES - (potValue - potZero) / (POT_HIGH - potZero) * (TOWER_ZERO_DEGREES - TOWER_LOW);
		//uint16_t tower_servo_pos = (potValue - potZero) / (POT_HIGH - potZero) * (TOWER_HIGH - TOWER_ZERO_DEGREES) + TOWER_ZERO_DEGREES;
		printf("\n\r Pot val: %lf, Tower servo pos: %d \r\n", potValue, tower_servo_pos);
		rotateServo(TOWER_SERVO, tower_servo_pos);
	}
//	if (potZero > 1.5) {
//		printf("\n\r Game Type A\r\n");
//		uint16_t tower_servo_pos = (potZero - potValue) / (potZero - POT_LOW) * (TOWER_ZERO_DEGREES - TOWER_LOW) + TOWER_ZERO_DEGREES;
//		printf("\n\r Pot val: %lf, Tower servo pos: %d \r\n", potValue, tower_servo_pos);
//		rotateServo(TOWER_SERVO, tower_servo_pos);
//	} else {
//		printf("\n\r Game Type B\r\n");
//		uint16_t tower_servo_pos = (potValue - potZero) / (POT_HIGH - potZero) * (TOWER_HIGH - TOWER_ZERO_DEGREES) + TOWER_ZERO_DEGREES;
//		printf("\n\r Pot val: %lf, Tower servo pos: %d \r\n", potValue, tower_servo_pos);
//		rotateServo(TOWER_SERVO, tower_servo_pos);
//	}
}

void initializeServos(void) {
		// Set the clock to run at 40MhZ using the PLL and 16MHz external crystal
	SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN
			| SYSCTL_XTAL_16MHZ);
  // initialize the timer sub-system and console I/O
  _HW_Timer_Init(ES_Timer_RATE_1mS);
	ADC0_InitSWTriggerSeq3(POT_CHAN); // Initializes the pot
	PWM_TIVA_Init();
  PWM_TIVA_SetFreq(50, 0);
  PWM_TIVA_SetFreq(50, 1);
  PWM_TIVA_SetPulseWidth(key_servo_pos, KEY_SERVO);
	PWM_TIVA_SetPulseWidth(tower_rotate_servo_pos, TOWER_ROTATE_SERVO);
	PWM_TIVA_SetPulseWidth(tower_servo_pos,TOWER_SERVO);
	PWM_TIVA_SetPulseWidth(flag_servo_pos,FLAG_SERVO);
}

void setTowerToZero(void) {
	rotateServo(TOWER_SERVO, TOWER_ZERO_DEGREES);
}


void rotateServo(uint8_t channel, uint16_t position) {
	if (!(position < MINUS_90_DEGREES || position > PLUS_90_DEGREES)) {
		if (channel == KEY_SERVO) {
			key_servo_pos = position;
			PWM_TIVA_SetPulseWidth(key_servo_pos, KEY_SERVO);
		} else if (channel == TOWER_ROTATE_SERVO) {
			tower_rotate_servo_pos = position;
			PWM_TIVA_SetPulseWidth(tower_rotate_servo_pos,TOWER_ROTATE_SERVO);
		} else if (channel == TOWER_SERVO) {
			tower_servo_pos = position;
			PWM_TIVA_SetPulseWidth(tower_servo_pos,TOWER_SERVO);
		} else if (channel == FLAG_SERVO) {
			flag_servo_pos = position;
			PWM_TIVA_SetPulseWidth(flag_servo_pos,FLAG_SERVO);
		}
	}
}

void rotateServoLeft(uint8_t channel) {
	if (channel == KEY_SERVO) {
		if (!(key_servo_pos - 10 < MINUS_90_DEGREES))
			key_servo_pos = key_servo_pos - 10;
		PWM_TIVA_SetPulseWidth(key_servo_pos, KEY_SERVO);
	} else if (channel == TOWER_ROTATE_SERVO) {
		if (!(tower_rotate_servo_pos - 15 < MINUS_90_DEGREES))
			tower_rotate_servo_pos = tower_rotate_servo_pos - 15;
		PWM_TIVA_SetPulseWidth(tower_rotate_servo_pos, TOWER_ROTATE_SERVO);
	} else if (channel == TOWER_SERVO) {
		if (!(tower_servo_pos - 2 < MINUS_90_DEGREES))
			tower_servo_pos = tower_servo_pos - 2;
		PWM_TIVA_SetPulseWidth(tower_servo_pos, TOWER_SERVO);
	} else if (channel == FLAG_SERVO) {
		if (!(flag_servo_pos - 10 < MINUS_90_DEGREES))
			flag_servo_pos = flag_servo_pos - 10;
		PWM_TIVA_SetPulseWidth(flag_servo_pos, FLAG_SERVO);
	}
}

void rotateServoRight(uint8_t channel) {
	if (channel == KEY_SERVO) {
		if (!(key_servo_pos + 10 > PLUS_90_DEGREES))
			key_servo_pos = key_servo_pos + 10;
		PWM_TIVA_SetPulseWidth(key_servo_pos, KEY_SERVO);
	} else if (channel == TOWER_ROTATE_SERVO) {
		if (!(tower_rotate_servo_pos + 15 > PLUS_90_DEGREES))
			tower_rotate_servo_pos = tower_rotate_servo_pos + 15;
		PWM_TIVA_SetPulseWidth(tower_rotate_servo_pos, TOWER_ROTATE_SERVO);
	} else if (channel == TOWER_SERVO) {
		if (!(tower_servo_pos + 2 > PLUS_90_DEGREES))
			tower_servo_pos = tower_servo_pos + 2;
		PWM_TIVA_SetPulseWidth(tower_servo_pos, TOWER_SERVO);
	} else if (channel == FLAG_SERVO) {
		if (!(flag_servo_pos + 10 > PLUS_90_DEGREES))
			flag_servo_pos = flag_servo_pos + 10;
		PWM_TIVA_SetPulseWidth(flag_servo_pos, FLAG_SERVO);
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
//		double potValue = getPotValue();
//		uint16_t tower_servo_pos = (potValue - POT_LOW) / (POT_HIGH - POT_LOW) * (TOWER_HIGH - TOWER_LOW) + TOWER_LOW;\
//		printf("\n\r Pot val: %lf, Tower servo pos: %d \r\n", potValue, tower_servo_pos);
//		rotateServo(TOWER_SERVO, tower_servo_pos);
		char input = getchar();
		switch (input) {
			case 'a':
				rotateServoLeft(TOWER_SERVO);
				printf("Servo Position is %d\r\n", tower_servo_pos);
				break;
			
			case 's':
				rotateServoRight(TOWER_SERVO);
				printf("Servo Position is %d\r\n", tower_servo_pos);
				break;
			
			case 'q':
				rotateServoLeft(FLAG_SERVO);
				printf("Servo Position is %d\r\n", flag_servo_pos);
				break;
			
			case 'w':
				rotateServoRight(FLAG_SERVO);
				printf("Servo Position is %d\r\n", flag_servo_pos);
				break;
			
			case 'e':
				lockKeys();
				printf("Locking Keys...\r\n");
				printf("Servo Position is %d\r\n", key_servo_pos);
				break;
			
			case 'r':
				unlockKeys();
				printf("Unlocking Keys...\r\n");
				printf("Servo Position is %d\r\n", key_servo_pos);
				break;
			
			case 't':
				raiseFlag();
				printf("Raising Flag...\r\n");
				printf("Servo Position is %d\r\n", flag_servo_pos);
				break;
			
			case 'y':
				lowerFlag();
				printf("Lowering Flag...\r\n");
				printf("Servo Position is %d\r\n", flag_servo_pos);
				break;
			
			case 'f':
				printf("Invoking Fear and Panic...\r\n");
				while (true) {
					rotateServoRight(TOWER_ROTATE_SERVO);

					printf("Servo Position is %d\r\n", tower_rotate_servo_pos);
					rotateServoLeft(TOWER_ROTATE_SERVO);
					printf("Servo Position is %d\r\n", tower_rotate_servo_pos);
				}
				break;
		}
	}
}
#endif
