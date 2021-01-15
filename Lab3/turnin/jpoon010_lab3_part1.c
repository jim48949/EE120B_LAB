/*	Author: lab
 *  Partner(s) Name: Jim Poon
 *	Lab Section:
 *	Assignment: Lab #3  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

int main(void) 
{
	/* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0x00; // Configure port A's 8 pins as inputs
	DDRB = 0x00; PORTB = 0x00; // Configure port A's 8 pins as inputs
	DDRC = 0xFF; PORTC = 0x00; // Configure port C's 8 pins as outputs
	unsigned char tmpA = 0x00; // Temporary variable to hold the value of A
	unsigned char tmpB = 0x00; // Temporary variable to hold the value of B
	unsigned char tmpC = 0x00; // Temporary variable to hold the value of C
	/* Insert your solution below */
	while (1) 
	{
		tmpA = PINA & 0xFF;
		tmpB = PINB & 0xFF;
		tmpC = 0x00;
		// Count the # of 1s on A and B
		if ((tmpA & 0x01) == 0x01) 
			tmpC = tmpC + 1;
		if ((tmpA & 0x02) == 0x02)
			tmpC = tmpC + 1;
		if ((tmpA & 0x04) == 0x04)
                	tmpC = tmpC + 1;
        	if ((tmpA & 0x08) == 0x08)
                	tmpC = tmpC + 1;
		if ((tmpA & 0x10) == 0x10)
                	tmpC = tmpC + 1;
        	if ((tmpA & 0x20) == 0x20)
                	tmpC = tmpC + 1;
		if ((tmpA & 0x40) == 0x40)
                	tmpC = tmpC + 1;
        	if ((tmpA & 0x80) == 0x80)
                	tmpC = tmpC + 1;
		if ((tmpB & 0x01) == 0x01)
                	tmpC = tmpC + 1;
        	if ((tmpB & 0x02) == 0x02)
                	tmpC = tmpC + 1;
        	if ((tmpB & 0x04) == 0x04)
                	tmpC = tmpC + 1;
        	if ((tmpB & 0x08) == 0x08)
                	tmpC = tmpC + 1;
        	if ((tmpB & 0x10) == 0x10)
                	tmpC = tmpC + 1;
        	if ((tmpB & 0x20) == 0x20)
                	tmpC = tmpC + 1;
        	if ((tmpB & 0x40) == 0x40)
                	tmpC = tmpC + 1;
        	if ((tmpB & 0x80) == 0x80)
                	tmpC = tmpC + 1;
		// Output 		
		PORTC = tmpC;
	}
	return 1;
}
