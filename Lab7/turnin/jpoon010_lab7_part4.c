/*	Author: lab
 *  Partner(s) Name: Jim Poon
 *	Lab Section:
 *	Assignment: Lab #7  Exercise #4
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *	Demo Link: 
 *	https://drive.google.com/file/d/1D_h3cFBGXftvfIE7KGJJEhfkaDAJmtnZ/view?usp=sharing
 *
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

void ADC_init()
{
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE); 
}

int main(void) {
   	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	unsigned short ADC_result = 0x0000;
	unsigned char tmpB = 0x00;
	
	ADC_init();
	
    	while (1) 
    	{
			ADC_result = ADC;
			if(ADC_result >= 0x91)
				tmpB = 0x01;
			else if(ADC_result < 0x91 && ADC_result >= 0x81)		
				tmpB = 0x03;
			else if(ADC_result < 0x81 && ADC_result >= 0x6F)		
				tmpB = 0x07;
			else if(ADC_result < 0x6F && ADC_result >= 0x5D)		
				tmpB = 0x0F;
			else if(ADC_result < 0x5D && ADC_result >= 0x4B)		
				tmpB = 0x1F;
			else if(ADC_result < 0x4B && ADC_result >= 0x39)		
				tmpB = 0x3F;
			else if(ADC_result < 0x39 && ADC_result >= 0x27)		
				tmpB = 0x7F;
			else
				tmpB = 0xFF;

			PORTB = tmpB;
    	}
    	return 1;
}
