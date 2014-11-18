#ifndef PASSWORD_GENERATOR_H
#define PASSWORD_GENERATOR_H

void generateRandomPassword(uint8_t* randomPassword);
void randomizePasswords(void);
uint8_t* getPassword(uint8_t num);
void printPassword(uint8_t* password);
bool checkPassword(uint8_t *guess);

#endif
