/*	Author: lab
 *  Partner(s) Name: Jim Poon
 *	Lab Section: 023
 *	Assignment: Lab #2  Exercise #1
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
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as outputs, initialize to 0s
	unsigned char tmpB = 0x00; // Temporary variable to hold the value of B
	unsigned char tmpA = 0x00; // Temporary variable to hold the value of A
	while(1) {
		// 1) Read input
		tmpA = PINA & 0x03;
		// 2) Perform computation
		// PA1 && PA0, PA1 && !PA0, !PA1 && !PA0
		if ((tmpA == 0x00) || (tmpA == 0x02) || (tmpA == 0x03)) 
		{ 
			// Set tmpB to bbbbbb00
			tmpB = (tmpB & 0xFC) | 0x00;
		} 
		else if(tmpA == 0x01)
		{
			// Set tmpB to bbbbbb01
			tmpB = (tmpB & 0xFC) | 0x01;
		}	
	// 3) Write output
	PORTB = tmpB;	
	}
	return 0;
}


