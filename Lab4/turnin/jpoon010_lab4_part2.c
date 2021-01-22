/*	Author: lab
 *  Partner(s) Name: Jim Poon
 *	Lab Section: 023
 *	Assignment: Lab #4  Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum States{SM_Init, SM_Start, SM_A0, SM_A1, SM_RESET} State;

unsigned char tmpA; // Temporary variable to hold the value of A
unsigned char tmpC; // Temporary variable to hold the value of C
unsigned char SM()
{	
	switch(State)
	{
		case SM_Init:
			State = SM_Start;
			tmpC = 0x07;
			break;
		case SM_Start:
			if(tmpA == 0x03)
				State = SM_RESET;
			else if(tmpA == 0x01)
			{	
				if(tmpC < 0x09)
				{
					tmpC++;
					State = SM_A0;	
				}
				else
					State = SM_A0;
			}	
			else if(tmpA == 0x02)
			{
				if(tmpC > 0x00)	
				{			
					tmpC--;
					State = SM_A1;
				}
				else
					State = SM_A1;
			}
			else if (tmpA == 0x03)
				State = SM_RESET;
			break;
		case SM_A0:
			if(tmpA == 0x01)
				State = SM_A0;
			else if( (tmpA|0x02) == 0x03)
			{
				if(tmpC > 0x00)				
					tmpC--;
				tmpC = 0x00;
				State = SM_RESET;
			}
			else
				State = SM_Start;	
			break;
		case SM_A1:
			if(tmpA == 0x02)
				State = SM_A1;
			else if( (tmpA|0x01) == 0x03)
			{
				if(tmpC < 0x09)
					tmpC++;	
				tmpC = 0x00;
				State = SM_RESET;
			}
			else
				State = SM_Start;	
			break;
		case SM_RESET:
			tmpC = 0x00;
			State = SM_Start;
			break;
		default:
			State = SM_Init;
	}
	return tmpC;
}

int main(void) {
    	/* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0x00; // Port A's 8 pins as inputs
	DDRC = 0xFF; PORTC = 0x00; // Port C's 8 pins as outputs
	tmpC = 0x00; tmpA = 0x00;	
	State = SM_Init;	
	
    	/* Insert your solution below */
    	while (1) 
	{
		tmpA = PINA;	
		PORTC = SM();	
    	}
    	return 1;
}
