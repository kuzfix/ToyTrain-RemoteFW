/*
 * LCD.h
 *
 * Created: 23.4.2013 22:14:06
 *  Author: maticpi
 */ 

#ifndef KBD_H_
#define KBD_H_

#include <avr/io.h>			//header file with register definitions
/* B1-B4,B6-B7
PE3
PE2
PD7
PD6
PD5
PE1
*/

#define BTN1	1
#define BTN2	2
#define BTN3	3
#define BTN4	4
#define BTN6	6
#define BTN7	7

#define BTN1_DDR	DDRE
#define BTN2_DDR	DDRE
#define BTN3_DDR	DDRD
#define BTN4_DDR	DDRD
#define BTN6_DDR	DDRD
#define BTN7_DDR	DDRE

#define BTN1_PORT	PORTE
#define BTN2_PORT	PORTE
#define BTN3_PORT	PORTD
#define BTN4_PORT	PORTD
#define BTN6_PORT	PORTD
#define BTN7_PORT	PORTE

#define BTN1_PIN	PINE
#define BTN2_PIN	PINE
#define BTN3_PIN	PIND
#define BTN4_PIN	PIND
#define BTN6_PIN	PIND
#define BTN7_PIN	PINE

#define BTN1_BIT	3
#define BTN2_BIT	2
#define BTN3_BIT	7
#define BTN4_BIT	6
#define BTN6_BIT	5
#define BTN7_BIT	1

void KBD_Init();
void KBD_Read();
char KBD_GetKey();
char KBD_GetReleasedKey();
void KBD_flush();
char KBD_isKeyStatePressed(char key);
char KBD_isKeyStateReleased(char key);

extern char lastkey;
extern char lastreleased;

#endif /* KBD_H_ */