/*	Author: lab
 *  Partner(s) Name: Jim Poon
 *	Lab Section:
 *	Assignment: Lab #5  Exercise #3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *	
 *	Demo Link: 
 *	https://drive.google.com/file/d/1gyUejz8pBtbfi8psvc3FIvel8e3vIIdu/view?usp=sharing
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum States{SM_Init, SM_Start, SM_Release, SM_Press} State;

unsigned char tmpA; // Temporary variable to hold the value of A
unsigned char tmpB; // Temporary variable to hold the value of C
unsigned char isODD;
unsigned char SM()
{	
	switch(State)
	{
		case SM_Init:
			State = SM_Start;
			tmpB = 0x01;
			isODD = 1;
			break;
		case SM_Start:
			if(tmpA == 1)
			{
				State = SM_Press;
				tmpB = 0x01;
			}
			break;
		case SM_Press:
			if(tmpA == 1)
				State = SM_Press;
			else if(tmpA == 0)
				State = SM_Release;
			break;
		case SM_Release:
			if(tmpA == 1)
			{
				State = SM_Press;
				if(tmpB < 0x20 && isODD)
					tmpB = tmpB << 2;
				else if(tmpB < 0x40 && !isODD)
					tmpB = tmpB << 2;
				if(isODD && tmpB >= 0x20)
				{
					isODD = 0;					
					tmpB = 0x02;
				}
				else if(!isODD && tmpB >= 0x40)
				{
					isODD = 1;					
					tmpB = 0x01;
				}
			}
			break;
		default:
			State = SM_Start;
	}
	return tmpB;
}

int main(void) {
    	/* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF; // Port A's 8 pins as inputs
	DDRB = 0xFF; PORTB = 0x00; // Port C's 8 pins as outputs
	tmpB = 0x00; tmpA = 0x00;	
	State = SM_Init;	
	
    	/* Insert your solution below */
    	while (1) 
	{
		tmpA = ~PINA & 0x01;	
		PORTB = SM();	
    	}
    	return 1;
}
