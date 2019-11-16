/*
 * LED.c
 *
 * Created: 24-Feb-16 11:38:24
 * Author : maticpi
 */ 

#include "LED.h"

void LED_Init()
{
	//PORT E Bits 0-3 LEDs, bits 4-7 not implemented
	LED_1Off();
	LED_2Off();
	LED_3Off();
	LED_4Off();
	LED_5Off();
	LED_6Off();
	LED_7Off();
	LED1_DDR |=  (1<<LED1_BIT);
	LED2_DDR |=  (1<<LED2_BIT);
	LED3_DDR |=  (1<<LED3_BIT);
	LED4_DDR |=  (1<<LED4_BIT);
	LED5_DDR |=  (1<<LED5_BIT);
	LED6_DDR |=  (1<<LED6_BIT);
	LED7_DDR |=  (1<<LED7_BIT);
}

void LED_AllOff(void)
{
	LED_1Off();
	LED_2Off();
	LED_3Off();
	LED_4Off();
	LED_5Off();
	LED_6Off();
	LED_7Off();
}

void LED_AllOn(void)
{
	LED_1On();
	LED_2On();
	LED_3On();
	LED_4On();
	LED_5On();
	LED_6On();
	LED_7On();
}

inline void LED_1On()  {LED1_PORT |=  (1<<LED1_BIT); }
inline void LED_1Off() {LED1_PORT &= ~(1<<LED1_BIT); }
inline void LED_1Tgl() {LED1_PIN   =  (1<<LED1_BIT); }

inline void LED_2On()  {LED2_PORT |=  (1<<LED2_BIT); }
inline void LED_2Off() {LED2_PORT &= ~(1<<LED2_BIT); }
inline void LED_2Tgl() {LED2_PIN   =  (1<<LED2_BIT); }

inline void LED_3On()  {LED3_PORT |=  (1<<LED3_BIT); }
inline void LED_3Off() {LED3_PORT &= ~(1<<LED3_BIT); }
inline void LED_3Tgl() {LED3_PIN   =  (1<<LED3_BIT); }

inline void LED_4On()  {LED4_PORT |=  (1<<LED4_BIT); }
inline void LED_4Off() {LED4_PORT &= ~(1<<LED4_BIT); }
inline void LED_4Tgl() {LED4_PIN   =  (1<<LED4_BIT); }

inline void LED_5On()  {LED5_PORT |=  (1<<LED5_BIT); }
inline void LED_5Off() {LED5_PORT &= ~(1<<LED5_BIT); }
inline void LED_5Tgl() {LED5_PIN   =  (1<<LED5_BIT); }

inline void LED_6On()  {LED6_PORT |=  (1<<LED6_BIT); }
inline void LED_6Off() {LED6_PORT &= ~(1<<LED6_BIT); }
inline void LED_6Tgl() {LED6_PIN   =  (1<<LED6_BIT); }

inline void LED_7On()  {LED7_PORT |=  (1<<LED7_BIT); }
inline void LED_7Off() {LED7_PORT &= ~(1<<LED7_BIT); }
inline void LED_7Tgl() {LED7_PIN   =  (1<<LED7_BIT); }
	
void LED_Set(uint8_t leds)
{
	if (leds & (1<<0)) LED_1On();
	if (leds & (1<<1)) LED_2On();
	if (leds & (1<<2)) LED_3On();
	if (leds & (1<<3)) LED_4On();
	if (leds & (1<<4)) LED_5On();
	if (leds & (1<<5)) LED_6On();
	if (leds & (1<<6)) LED_7On();
}

void LED_Clr(uint8_t leds)
{
	if (leds & (1<<0)) LED_1Off();
	if (leds & (1<<1)) LED_2Off();
	if (leds & (1<<2)) LED_3Off();
	if (leds & (1<<3)) LED_4Off();
	if (leds & (1<<4)) LED_5Off();
	if (leds & (1<<5)) LED_6Off();
	if (leds & (1<<6)) LED_7Off();
}

uint8_t LED_Get()
{
	uint8_t state=0;
	if (LED1_PORT & (1<<LED1_PIN)) state |= (1<<0);
	if (LED2_PORT & (1<<LED2_PIN)) state |= (1<<1);
	if (LED3_PORT & (1<<LED3_PIN)) state |= (1<<2);
	if (LED4_PORT & (1<<LED4_PIN)) state |= (1<<3);
	if (LED5_PORT & (1<<LED5_PIN)) state |= (1<<4);
	if (LED6_PORT & (1<<LED6_PIN)) state |= (1<<5);
	if (LED7_PORT & (1<<LED7_PIN)) state |= (1<<6);
	return state;
}
