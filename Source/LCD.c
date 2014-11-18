/****************************************************************************
 Module
   LCD.c

 Description
   This file contains the functions that will be needed to initialize the LCD
	 and to write to the LCD
	 
	 Currently, I will try to send all the information (like RS, D0, etc. pins),
	 pulse the shift register to send this to the LCD, then pulse the shift
	 reigster again, in quick succession with E being set low and hi.
****************************************************************************/
//#define TEST

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
#include "LCD.h"
#include "passwordGenerator.h"

// not as sure if these are absolutley needed
#include "driverlib/gpio.h"
//#include "driverlib/interrupt.h"
//#include "utils/uartstdio.h"

#define ALL_BITS (0xff <<2)

// definitions for array bit numbers
#define E_HI BIT1HI
#define E_LO BIT1LO
#define RW_HI BIT2HI 
#define RW_LO BIT2LO 
#define RS_HI BIT3HI //data
#define RS_LO BIT3LO
#define D7_HI BIT4HI
#define D7_LO BIT4LO
#define D6_HI BIT5HI
#define D6_LO BIT5LO
#define D5_HI BIT6HI
#define D5_LO BIT6LO
#define D4_HI BIT7HI
#define D4_LO BIT7LO

#define BIT(x) (1<<(x))

//the order goes: D4-D5-D6-D7-RS-RW-E-empty
//on the shift register: QB-QC-QD-QE-QF-QG-QH
//bit numbering goes:7-6-5-4-3-2-1-0
// so bit 0 is alwasys uselss (empty)
/****************************************************************************
  Change these values if ports C4 (data), C5 (SCK), C6 (RCK) are
	not being used for the shift register
****************************************************************************/
// CHANGE THE PORTS IF NECESSARY
#define LCD_PORT_DEC SYSCTL_RCGCGPIO_R2 //port C
#define LCD_PORT GPIO_PORTC_BASE // port C base
#define LCD_DATA GPIO_PIN_4 // pin 4
#define LCD_SCK GPIO_PIN_5 // pin 5
#define LCD_RCK GPIO_PIN_6 // pin 6

char LCD;
static uint8_t messageNumber = 0;

// shift register functions
// probably want to make SCKPulse and RCKPulse take an input of
// LED or LCD so that we only need to write the function once
//void ShiftRegLCD (char LCD[8]);
void SCKPulseLCD (void); // transfer bit to shift register
void RCKPulseLCD (void); // transfer bits to the world
void sendToLCD (char Data); // takes 8 bit input and pulses E to input into LCD
void sendToShiftReg (char Data); // sends the 8 bit input to shift register

void LCDInit (void); //initializes LP and then 4 bit mode of LCD
void LCDputchar (char letter);


void SCKPulseLCD (void) {
	// pulses SCK pin HI
	HWREG(LCD_PORT+(GPIO_O_DATA + ALL_BITS)) |= LCD_SCK; 
	//kill time
	for (int i = 1; i < 50; i ++){
		HWREG(SYSCTL_RCGCGPIO);
	}
	// pulses SCK pin LO
	HWREG(LCD_PORT+(GPIO_O_DATA + ALL_BITS)) &= ~LCD_SCK; 
}

void RCKPulseLCD (void) {
	// pulses RCK pin HI
	HWREG(LCD_PORT+(GPIO_O_DATA + ALL_BITS)) |= LCD_RCK; 
	for (int i = 1; i < 50; i ++){
		HWREG(SYSCTL_RCGCGPIO);
	}
	// pulses SCK pin LO
	HWREG(LCD_PORT+(GPIO_O_DATA + ALL_BITS)) &= ~LCD_RCK; 	
}

void sendToShiftReg (char Data) {
	char send = 0;
	for (int i= 0; i <8; i ++) {
		send = (!! ((Data << i) & (0x80))); //takes left most bit, and returns 1 if it is 1, and 0 if it is 0
		if (send == 1){
			HWREG(LCD_PORT + GPIO_O_DATA + ALL_BITS) |= LCD_DATA;
		}
		else if (send == 0) {
			HWREG(LCD_PORT + GPIO_O_DATA + ALL_BITS) &= ~(LCD_DATA);
		}
		else {
			puts ("entered if send is neither 1 nor 0");
			puts("\n\r sendToShiftReg send value not 0 or 1 \r\n");
		}
		SCKPulseLCD();
	}
	RCKPulseLCD();
}

void sendToLCD (char Data) {
	sendToShiftReg(Data);
	Data |= E_HI;
	sendToShiftReg(Data);
	wait(1);
	Data &= E_LO;
	sendToShiftReg(Data);
}

void LCDInit (void) {
	/* LaunchPad init for ports C 4, 5 and 6 and 7*/
	HWREG(SYSCTL_RCGCGPIO) |= LCD_PORT_DEC;
	// kill time
	HWREG(SYSCTL_RCGCGPIO);
	//set C4-7 as outputs
	HWREG(LCD_PORT + GPIO_O_DEN)|= (LCD_DATA | LCD_SCK | LCD_RCK);
	HWREG(LCD_PORT + GPIO_O_DIR) |= (LCD_DATA | LCD_SCK | LCD_RCK);
	
	/* initialize timer */
	_HW_Timer_Init(ES_Timer_RATE_1mS);
	
	/*
	 LCD Initialization begins
	 Wait for more than 15 ms after VCC rises to 4.5 V
	 */
	wait (110);
	LCD = 0;
	
	// sends 000011 3 times, with 5mS in between each
	for (int i = 1; i < 4; i ++) {
		
		LCD |= (D5_HI | D4_HI);
		
		sendToLCD(LCD); //pulse to send 000011 both through the shfit register and the LCD
		wait(5); //wait 5mS between each loop
	}
	
	/*
	 * Function set ( Set interface to be 4 bits long. )
	 * Interface is 8 bits in length.
	 */
	//sending 00010
	LCD &= D4_LO; //just changed D4 to LO
	sendToLCD(LCD);
	wait (1);
	
	/*
	 * Function set ( Interface is 4 bits long. Specify
	 * the number of display lines and character font. )
	 * The number of display lines and character font
	 * can not be changed after this point.
	 */
	//sending 00010 
	sendToLCD(LCD);
	//wait (2);
	/*sending 000NF** 
	 * N is on DB7, 0 for one line
	 * F is on DB6, 1 for bigger font
	 */
	LCD &= D7_LO;
	LCD |= D6_HI;
	sendToLCD(LCD);
	wait (1);
	
	/*
	 * Display off
	*/
	//sending 000000
	LCD = 0;
	sendToLCD(LCD);
	//wait (2);
	//sending 001000
	LCD |= D7_HI;
	sendToLCD(LCD);
	wait (1);
	
	/*
	 * Display clear
	*/
	//sending 000000
	LCD &= D7_LO;
	sendToLCD(LCD);
	//wait (2);
	//sending 0001
	LCD |= D4_HI;
	sendToLCD(LCD);
	wait (1);
	
	/* 
	 * Entry mode set
	 */
	//sending 0000
	LCD &= D4_LO;
	sendToLCD(LCD);
	//wait (2);
	/* sending 0000(I/D)S
	 * I/D is on DB5, 1 is to increment cursor
	 * S is on DB4, 1 is to shift screen 
	 */
	LCD |= D4_HI;
	LCD |= D5_HI;
	sendToLCD(LCD);
	wait (1);
	
	// send 0000, 1111 to turn back on
	LCD &= D4_LO;
	LCD &= D5_LO;
	sendToLCD(LCD);
	//wait (2);
	
	LCD |= (D4_HI | D5_HI | D6_HI | D7_HI);
	sendToLCD(LCD);
	wait (1);
	
	/* sets cursor to start at right side
	 */
	
	LCD &= D4_LO;
	LCD &= D5_LO;
	LCD &= D6_LO;
	LCD &= D7_LO;
	
	sendToLCD(LCD);
		
	LCD |= (D4_HI | D5_HI | D6_HI | D7_HI);
	
	sendToLCD(LCD);
	
	wait(1);
	
	 // Sets entry mode so that the display shifts when a byte data is written
	//sending 0000
//	LCD &= D4_LO;
//	sendToLCD(LCD);
//	//sending 0111
//	LCD |= D4_HI;
//	LCD |= D5_HI;
//	LCD |= D6_HI;
//	sendToLCD(LCD);
//	wait (1);
	
}

void clearLCD(void) {
	/*
	 * Display clear
	*/
	//sending 000000
	LCD = 0;
	sendToLCD(LCD);
	//wait (2);
	//sending 0001
	LCD |= D4_HI;
	sendToLCD(LCD);
	wait (1);
}

// * LCD = D4-D5-D6-D7-RS-RW-E-empty
void LCDputchar (char letter) {
	//puts("\n\r in LCDputchar p\r\n");
	
	//printf("\n\r Lower Nibble is : %#x \n\r", letter & 0x0f);// & 0xF0);
	//printf("\n\r Upper Nibble is : %#x \n\r", (letter>>4) & 0x0f);// & 0xF0);

	uint8_t char_bits[8] = {0, 0, 0, 0, 0, 0 ,0 ,0};
	for (uint8_t i = 0; i < 8; i++) {
		if ((letter & 0x01) == 0x01)
			char_bits[i] = 1;
		letter = letter >> 1;
	}
	
	for (uint8_t i = 0; i < 8; i++) {
		//printf("Bit %d is %d\r\n", i, char_bits[i]);
	}
	
	// Clears the data pin bits of LCD but keeps RS, RW, etc.
	LCD &= D4_LO;
	LCD &= D5_LO;
	LCD &= D6_LO;
	LCD &= D7_LO;
	LCD |= RS_HI; //data mode
	// Sets upper 4 bits (in reverse order) to be sent
	if (char_bits[4])
		LCD |= (0x80); // Set 1000000
	if (char_bits[5])
		LCD |= (0x40); // Set 0100000
	if (char_bits[6])
		LCD |= (0x20); // Set 0010000
	if (char_bits[7])
		LCD |= (0x10); // Set 0001000
	//printf("\n\r Upper Nibble Reversed is: %#x \n\r", LCD);// & 0xF0);
	sendToLCD (LCD); //sends upper 4 bits
	wait(1);
	
	// Clears the data pin bits of LCD but keeps RS, RW, etc.
	LCD &= D4_LO;
	LCD &= D5_LO;
	LCD &= D6_LO;
	LCD &= D7_LO;
	// Sets lower 4 bits (in reverse order) to be sent
	if (char_bits[0])
		LCD |= (0x80); // Set 1000000
	if (char_bits[1])
		LCD |= (0x40); // Set 0100000
	if (char_bits[2])
		LCD |= (0x20); // Set 0010000
	if (char_bits[3])
		LCD |= (0x10); // Set 0001000
	//printf("\n\r Lower Nibble Reversed is: %#x \n\r", LCD);// & 0xF0);
	sendToLCD (LCD); //sends upper 4 bits
	wait(1);
}

void resetLCDmessage(void) {
	messageNumber = 1;
}

void printLCDmessage(void) {
	char c[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
	switch (messageNumber) {
		case 1:
			clearLCD();
			LCDputchar('W');
			LCDputchar('e');
			LCDputchar('l');
			LCDputchar('c');
			LCDputchar('o');
			LCDputchar('m');
			LCDputchar('e');
			LCDputchar(' ');
			LCDputchar('D');
			LCDputchar('r');
			LCDputchar('E');
			LCDputchar('d');
			LCDputchar(' ');
			LCDputchar(' ');
			LCDputchar(' ');
			LCDputchar(' ');
			
			messageNumber = 2;
			break;
		
		case 2:
			clearLCD();
			LCDputchar('P');
			LCDputchar('a');
			LCDputchar('s');
			LCDputchar('s');
			LCDputchar('c');
			LCDputchar('o');
			LCDputchar('d');
			LCDputchar('e');
			LCDputchar('s');
			LCDputchar('.');
			LCDputchar('.');
			LCDputchar('.');
			LCDputchar(' ');
			LCDputchar(' ');
			LCDputchar(' ');
			LCDputchar(' ');
			LCDputchar(' ');
			messageNumber = 3;
			break;
		
		case 3:
			clearLCD();
			LCDputchar( c[getPassword(1)[0]]);
			LCDputchar( c[getPassword(1)[1]]);
			LCDputchar( c[getPassword(1)[2]]);
			LCDputchar( c[getPassword(1)[3]]);
			LCDputchar( c[getPassword(1)[4]]);
			LCDputchar(' ');
			LCDputchar(' ');
			LCDputchar(' ');
			LCDputchar(' ');
			LCDputchar(' ');
			LCDputchar(' ');
			LCDputchar(' ');
			LCDputchar(' ');
			messageNumber = 4;
			break;
			
		case 4:
			clearLCD();
			LCDputchar( c[getPassword(2)[0]]);
			LCDputchar( c[getPassword(2)[1]]);
			LCDputchar( c[getPassword(2)[2]]);
			LCDputchar( c[getPassword(2)[3]]);
			LCDputchar( c[getPassword(2)[4]]);
			LCDputchar(' ');
			LCDputchar(' ');
			LCDputchar(' ');
			LCDputchar(' ');
			LCDputchar(' ');
			LCDputchar(' ');
			LCDputchar(' ');
			LCDputchar(' ');
			messageNumber = 5;
			break;
		
		case 5:
			clearLCD();
			LCDputchar( c[getPassword(3)[0]]);
			LCDputchar( c[getPassword(3)[1]]);
			LCDputchar( c[getPassword(3)[2]]);
			LCDputchar( c[getPassword(3)[3]]);
			LCDputchar( c[getPassword(3)[4]]);
			LCDputchar(' ');
			LCDputchar(' ');
			LCDputchar(' ');
			LCDputchar(' ');
			LCDputchar(' ');
			LCDputchar(' ');
			LCDputchar(' ');
			LCDputchar(' ');
			messageNumber = 6;
			break;
			
		case 6:
			clearLCD();
			LCDputchar( c[getPassword(4)[0]]);
			LCDputchar( c[getPassword(4)[1]]);
			LCDputchar( c[getPassword(4)[2]]);
			LCDputchar( c[getPassword(4)[3]]);
			LCDputchar( c[getPassword(4)[4]]);
			LCDputchar(' ');
			LCDputchar(' ');
			LCDputchar(' ');
			LCDputchar(' ');
			LCDputchar(' ');
			LCDputchar(' ');
			LCDputchar(' ');
			LCDputchar(' ');
			messageNumber = 1;
			break;
	}
}

void printAuthorizedMessage(void) {
			clearLCD();
			LCDputchar('A');
			LCDputchar('u');
			LCDputchar('t');
			LCDputchar('h');
			LCDputchar('o');
			LCDputchar('r');
			LCDputchar('i');
			LCDputchar('z');
			LCDputchar('e');
			LCDputchar('d');
			LCDputchar('!');
			LCDputchar(' ');
			LCDputchar(' ');
			LCDputchar(' ');
}


void printIncorrectMessage(void) {
			clearLCD();
			LCDputchar('I');
			LCDputchar('n');
			LCDputchar('c');
			LCDputchar('o');
			LCDputchar('r');
			LCDputchar('r');
			LCDputchar('e');
			LCDputchar('c');
			LCDputchar('t');
			LCDputchar('.');
			LCDputchar('.');
			LCDputchar('.');
}

void printArmedMessage(void) {
			clearLCD();
			LCDputchar('A');
			LCDputchar('r');
			LCDputchar('m');
			LCDputchar('e');
			LCDputchar('d');
			LCDputchar(' ');
			LCDputchar(' ');
			LCDputchar(' ');
			LCDputchar(' ');
			LCDputchar(' ');
			LCDputchar(' ');
			LCDputchar(' ');
			LCDputchar(' ');
			LCDputchar(' ');
			LCDputchar(' ');
			LCDputchar(' ');
}

#ifdef TEST 
/* test Harness for testing this module */ 
#include "termio.h" 
int main(void) 
{ 
	TERMIO_Init(); 
	puts("\n\r In test harness of LCD\r\n");
	
	LCDInit();

	LCDputchar('W');
	LCDputchar('e');
	LCDputchar('l');
	LCDputchar('c');
	LCDputchar('o');
	LCDputchar('m');
	LCDputchar('e');
	LCDputchar(' ');
	LCDputchar('D');
	LCDputchar('r');
	//LCDputchar('3');
	return 0;
}
#endif
