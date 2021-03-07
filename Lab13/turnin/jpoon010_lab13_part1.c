/*	Author: lab
 *  Partner(s) Name: Jim Poon
 *	Lab Section:
 *	Assignment: Lab #13  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *	Demo Link: 
 *	https://drive.google.com/file/d/1GzzjtQ661QHWNbV--3t5kBNa1WzzxOaj/view?usp=sharing
 *
 */
#include <avr/io.h>
#include "timer.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

void A2D_init() 
{
      ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADEN: Enables analog-to-digital conversion
	// ADSC: Starts analog-to-digital conversion
	// ADATE: Enables auto-triggering, allowing for constant
	//	    analog to digital conversions.
}


int main(void) {
	
	DDRB = 0xFF; PORTB = 0x00;
	DDRD = 0xFF; PORTD = 0x00;

	unsigned char tmpB = 0x01;
	unsigned char tmpD = 0x01;
	unsigned short ADC_result = 0x0000;
	A2D_init();

    	while (1) 
	{
		ADC_result = ADC;
		tmpB = (char)ADC_result;
		tmpD = (char)(ADC_result >> 8);	

		PORTB = tmpB;
		PORTD = tmpD;
    	}
    	return 1;
}
