/*	Author: lab
 *  Partner(s) Name: Jim Poon
 *	Lab Section: 023
 *	Assignment: Lab #4  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum States{SM_PressON, SM_ReleaseON, SM_PressOFF, SM_ReleaseOFF} State;

unsigned char tmpA;

unsigned char SM()
{
	unsigned char tmpB = 0x00;
	switch(State)
	{
		case SM_ReleaseOFF:
			if(tmpA == 0x01)
			{
				State = SM_PressON;
				tmpB = 0x02;
			}		
			else if(tmpA == 0x00)
			{
				State = SM_ReleaseOFF;
				tmpB = 0x01;
			}
			break;
		case SM_PressON:
			if(tmpA == 0x00)
			{
				State = SM_ReleaseON;
				tmpB = 0x02;
			}
			else if(tmpA == 0x01)
			{
				State = SM_PressON;
				tmpB = 0x02;
			}
			break;
		case SM_ReleaseON:
			if(tmpA == 0x00)
			{
				State = SM_ReleaseON;
				tmpB = 0x02;
			}
			else if(tmpA == 0x01)
			{
				State = SM_PressOFF;
				tmpB = 0x01;
			}
			break;
		case SM_PressOFF:
			if(tmpA == 0x00)
			{
				State = SM_ReleaseOFF;
				tmpB = 0x01;
			}
			else if(tmpA == 0x01)
			{
				State = SM_PressOFF;
				tmpB = 0x01;
			}
			break;
		default:
			State = SM_ReleaseOFF;
			tmpB = 0x01;
	}
	return tmpB;
}

int main(void) {
    	/* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0x00; // Port A's 8 pins as inputs
	DDRB = 0xFF; PORTB = 0x00; // Port B's 8 pins as outputs
	//unsigned char tmpA; // Temporary variable to hold the value of A
	
	State = SM_ReleaseOFF;
	
    	/* Insert your solution below */
    	while (1) 
	{
		tmpA = PINA;	
		PORTB = SM();	
    	}
    	return 1;
}
