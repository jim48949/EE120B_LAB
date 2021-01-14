/*	Author: lab
 *  Partner(s) Name: Jim Poon
 *	Lab Section: 023
 *	Assignment: Lab #2  Exercise #4
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
	DDRA = 0x00; PORTA = 0x00; // Configure port A's 8 pins as inputs
	DDRB = 0x00; PORTB = 0x00; // Configure port A's 8 pins as inputs
	DDRC = 0x00; PORTC = 0x00; // Configure port C's 8 pins as inputs
	DDRD = 0xFF; PORTD = 0x00; // Configure port D's 8 pins as outputs
	unsigned char tmpA = 0x00; // Temporary variable to hold the value of A
	unsigned char tmpB = 0x00; // Temporary variable to hold the value of B
	unsigned char tmpC = 0x00; // Temporary variable to hold the value of C
	unsigned char tmpD = 0x00; // Temporary variable to hold the value of D
	unsigned char sum = 0x0000;
	while(1) {
		// 1) Read input
		tmpA = PINA & 0xFF;
		tmpB = PINB & 0xFF;
		tmpC = PINC & 0xFF;
		tmpD = 0x00;
		sum = 0x0000;
		// 2) Perform computation			
		// If total weight exceeds 140 (0x8C)
		sum = tmpA+tmpB+tmpC;
		if(sum>=0x8C)
			tmpD = tmpD | 0x01;
		// If |(A-C)| > 80 (0x50)
		if(tmpA > tmpC)
		{
			if((tmpA-tmpC) > 0x50)
				tmpD = tmpD | 0x02;
		}
		else if(tmpA < tmpC)
		{
			if((tmpC-tmpA) > 0x50)
				tmpD = tmpD | 0x02;
		}
		// Display the most significant 6 bits on D7-D2
		// Max 255*3 = 765 (0x2FD) < 0x300 (768)
		tmpD = tmpD | (sum>>2);			
		// 3) Write output
		PORTD = tmpD;	
	}
	return 0;
}


