/*	Author: lab
 *  Partner(s) Name: Jim Poon
 *	Lab Section:
 *	Assignment: Lab #7  Exercise #3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *	Demo Link: 
 *	https://drive.google.com/file/d/1XX7yVyP0bh1D3fNE_xJirZZPBBxeC41r/view?usp=sharing
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
	DDRD = 0xFF; PORTD = 0x00;
	unsigned short ADC_result = 0x0000;
	unsigned char tmpB = 0x01;
	//unsigned char tmpD = 0x01;
	unsigned char max = 0x9F >> 1;
	
	ADC_init();
	
    	while (1) 
    	{
			ADC_result = ADC;
			if(ADC_result >= max)
				tmpB = 0;
			else
				tmpB = 0x01;
			PORTB = tmpB;
    	}
    	return 1;
}
