/*	Author: lab
 *  Partner(s) Name: Jim Poon
 *	Lab Section:
 *	Assignment: Lab #5  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0x00; // Configure port A's 8 pins as inputs
	DDRC = 0xFF; PORTC = 0x00; // Configure port C's 8 pins as outputs
	unsigned char tmpA = 0x00; // Temporary variable to hold the value of A
	unsigned char tmpC = 0x00; // Temporary variable to hold the value of C
	unsigned char safeSys = 0x00; 
	while (1) 
	{
		tmpA = ~PINA & 0x0F;
		tmpC = 0x00;
		
		if((tmpA&0x0F)==0x0F || (tmpA&0x0E)==0x0E || (tmpA&0x0D)==0x0D)
			tmpC = tmpC | 0x3F;
		else if((tmpA&0x0C)==0x0C || (tmpA&0x0B)==0x0B || (tmpA&0x0A)==0x0A)
			tmpC = tmpC | 0x3E;
		else if((tmpA&0x09)==0x09 || (tmpA&0x08)==0x08 || (tmpA&0x07)==0x07)
			tmpC = tmpC | 0x3C;
		else if((tmpA&0x06)==0x06 || (tmpA&0x05)==0x05)
			tmpC = tmpC | 0x38;
		else if((tmpA&0x04)==0x04 || (tmpA&0x03)==0x03)
			tmpC = tmpC | 0x30;
		else if((tmpA&0x02)==0x02 || (tmpA&0x01)==0x01)
			tmpC = tmpC | 0x20;

		if(tmpA <= 0x04)
			tmpC = tmpC | 0x40;
			

		// Output 		
		PORTC = tmpC;
	}
	return 1;
}
