/*
 * MLLED.h
 *
 * Created: 24-Feb-16 12:29:51
 *  Author: maticpi
 */ 

#ifndef LED_H_
#define LED_H_

#include <avr/io.h>

/*
PE0
PD4
PD3
PD2
PC5
PC4
PC3
*/

#define LED1_DDR	DDRE
#define LED2_DDR	DDRD
#define LED3_DDR	DDRD
#define LED4_DDR	DDRD
#define LED5_DDR	DDRC
#define LED6_DDR	DDRC
#define LED7_DDR	DDRC

#define LED1_PORT	PORTE
#define LED2_PORT	PORTD
#define LED3_PORT	PORTD
#define LED4_PORT	PORTD
#define LED5_PORT	PORTC
#define LED6_PORT	PORTC
#define LED7_PORT	PORTC

#define LED1_PIN	PINE
#define LED2_PIN	PIND
#define LED3_PIN	PIND
#define LED4_PIN	PIND
#define LED5_PIN	PINC
#define LED6_PIN	PINC
#define LED7_PIN	PINC

#define LED1_BIT	0
#define LED2_BIT	4
#define LED3_BIT	3
#define LED4_BIT	2
#define LED5_BIT	5
#define LED6_BIT	4
#define LED7_BIT	3

void LED_Init();
void LED_AllOff();
void LED_AllOn();

void LED_1On();
void LED_1Off();
void LED_1Tgl();
void LED_2On();
void LED_2Off();
void LED_2Tgl();
void LED_3On();
void LED_3Off();
void LED_3Tgl();
void LED_4On();
void LED_4Off();
void LED_4Tgl();
void LED_5On();
void LED_5Off();
void LED_5Tgl();
void LED_6On();
void LED_6Off();
void LED_6Tgl();
void LED_7On();
void LED_7Off();
void LED_7Tgl();

void LED_Set(uint8_t leds);
void LED_Clr(uint8_t leds);
uint8_t LED_Get();

#endif /* LED_H_ */