/*	Author: lab
 *  Partner(s) Name: Jim Poon
 *	Lab Section: 023
 *	Assignment: Lab #2  Exercise #3
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
	DDRC = 0xFF; PORTC = 0x00; // Configure port C's 8 pins as outputs, initialize to 0s
	unsigned char cntavail; // Temporary variable to hold the value of C
	unsigned char tmpA = 0x00; // Temporary variable to hold the value of A
	while(1) {
		// 1) Read input
		cntavail = 4;
		tmpA = PINA & 0x0F;
		// 2) Perform computation			
		if((tmpA&0x08) == 0x08)
		{
			cntavail = cntavail-1;
		}
		if((tmpA&0x04) == 0x04)
		{
			cntavail = cntavail-1;
		}
		if((tmpA&0x02) == 0x02)
		{
			cntavail = cntavail-1;
		}
		if((tmpA&0x01) == 0x01)
		{
			cntavail = cntavail-1;
		}
		
		// If the lot is full
		if(!cntavail)
		{
			cntavail = 0x80;
		}		

		// 3) Write output
		PORTC = cntavail;	
	}
	return 0;
}


