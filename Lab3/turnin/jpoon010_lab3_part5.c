/*	Author: lab
 *  Partner(s) Name: Jim Poon
 *	Lab Section:
 *	Assignment: Lab #3  Exercise #5
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
	DDRD = 0x00; PORTD = 0x00; // Port D's 8 pins as inputs
	DDRB = 0xF0; PORTB = 0x00; // Port B's first 4 as output and last 4 as inputs
	unsigned char tmpD = 0x00; // Temporary variable to hold the value of D
	unsigned char tmpB = 0x00; // Temporary variable to hold the value of B
	unsigned short total = 0x0000;
	unsigned char output = 0x00;
	/* Insert your solution below */
	while (1) 
	{
		tmpD = PIND & 0xFF;
		tmpB = PINB & 0x0F;
		output = 0x00;
		total = tmpD << 1;
		total = total | tmpB;
		
		if(total >= 0x0046)
			output = output | 0x02;
		else if(total > 0x0005 && total < 0x0046)
			output = output | 0x04;
		// Output 		
		PORTB = output;
	}
	return 1;
}
