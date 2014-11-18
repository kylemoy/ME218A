//#define TEST
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "passwordGenerator.h"
#include "ADCSWTrigger.h"


#define MAX_PASS_LENGTH 5
#define NUM_PASSWORDS 4

static uint8_t correctPassword = 0;
static uint8_t password1[MAX_PASS_LENGTH];
static uint8_t password2[MAX_PASS_LENGTH];
static uint8_t password3[MAX_PASS_LENGTH];
static uint8_t password4[MAX_PASS_LENGTH];

void generateRandomPassword(uint8_t* randomPassword) {
	for (uint8_t i = 0; i < MAX_PASS_LENGTH; i++) {
		randomPassword[i] = rand() % 10;
	}
}

void randomizePasswords(void) {
	srand(ADC0_InSeq3());
	generateRandomPassword(password1);
	generateRandomPassword(password2);
	generateRandomPassword(password3);
	generateRandomPassword(password4);
	correctPassword = rand() % NUM_PASSWORDS + 1;
}

uint8_t* getPassword(uint8_t num) {
	switch (num) {
		case 1:
			return password1;
		case 2:
			return password2;
		case 3:
			return password3;
		case 4:
			return password4;
	}
	return NULL;
}


void printPassword(uint8_t* password) {
	for (uint8_t i = 0; i < MAX_PASS_LENGTH; i++) {
		printf("%d ", password[i]);
	}
	printf("\r\n");
}

bool compareIntArrays(uint8_t *array1, uint8_t *array2) {
	if (sizeof(array1) != sizeof(array2))
		return false;
	else {
		for (uint8_t i = 0; i < sizeof(array1); i++) {
			if (array1[i] != array2[i])
				return false;
		}
		return true;
	}
}

bool checkPassword(uint8_t *guess) {
	switch (correctPassword) {
		case 1:
			if (compareIntArrays(password1, guess))
				return true;
			break;
		case 2:
			if (compareIntArrays(password2, guess))
				return true;
			break;
		case 3:
			if (compareIntArrays(password3, guess))
				return true;
			break;
		case 4:
			if (compareIntArrays(password4, guess))
				return true;
			break;
	}
	return false;
}

#ifdef TEST 
/* test Harness for testing password module*/ 
#include "termio.h" 
int main(void) 
{ 
	TERMIO_Init(); 
	printf("\n\r In test harness of passwords\r\n");
	/* Intializes random number generator */
  srand(rand());
	randomizePasswords();
	printPassword(getPassword(1));
	printPassword(getPassword(2));
	printPassword(getPassword(3));
	printPassword(getPassword(4));
	printf("\r\nThe correct password is %d\r\n\r\n", correctPassword);
	if (checkPassword(getPassword(1)))
		printf("Password1 is correct\r\n");
	else
		printf("Password1 is incorrect\r\n");
	if (checkPassword(getPassword(2)))
		printf("Password2 is correct\r\n");
	else
		printf("Password2 is incorrect\r\n");
	if (checkPassword(getPassword(3)))
		printf("Password3 is correct\r\n");
	else
		printf("Password3 is incorrect\r\n");
	if (checkPassword(getPassword(4)))
		printf("Password4 is correct\r\n");
	else
		printf("Password4 is incorrect\r\n");
		
	// Int to Character Stuff
	char c[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
	for (int i = 0; i < 5; i++) {
		printf("%c\r\n", c[getPassword(1)[i]]);
	}
}
#endif
