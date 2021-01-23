/*	Author: lab
 *  Partner(s) Name: Jim Poon
 *	Lab Section: 023
 *	Assignment: Lab #4  Exercise #3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum States{SM_Init, S0_Start, S1_A2P, S2_A2R, S3_UNLOCK, S4_LOCK} State;

unsigned char tmpA; // Temporary variable to hold the value of A
unsigned char tmpB; // Temporary variable to hold the value of B
unsigned char tmpA1;
unsigned char tmpA2;
unsigned char tmpA7;

unsigned char SM()
{	
	tmpA1 = tmpA & 0x02;
	tmpA2 = tmpA & 0x04;
	tmpA7 = tmpA & 0x80;
	switch(State)
	{
		case SM_Init:
			State = S0_Start;
			break;
		case S0_Start:
			if(tmpA2)
				State = S1_A2P;
			else if(tmpA7)
			{
				tmpB = 0x00;				
				State = S4_LOCK;
			}
			else
				State = S0_Start;
			break;
		case S1_A2P:
			if(tmpA2 && !tmpA1)				
				State = S1_A2P;
			else if(!tmpA2 && !tmpA1)
				State = S2_A2R;
			else
				State = S0_Start;
			break;
		case S2_A2R:
			if(tmpA1 && !tmpA2)
			{			
				tmpB = 0x01;				
				State = S3_UNLOCK;
			}
			else if(!tmpA1 && !tmpA2 && !tmpA7)
				State = S2_A2R;
			else
				State =	S0_Start;
			break;
		case S3_UNLOCK:
			tmpB = 0x01;
			if(tmpA1 && !tmpA2)
				State = S3_UNLOCK;
			else
				State =	S0_Start;
			break;
		case S4_LOCK:
			tmpB = 0x00;
			if(tmpA7)
				State = S4_LOCK;
			else
				State =	S0_Start;
			break;
		default:
			State = SM_Init;
	}
	return tmpB;
}

int main(void) {
    	/* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0x00; // Port A's 8 pins as inputs
	DDRB = 0xFF; PORTB = 0x00; // Port B's 8 pins as outputs
 	tmpA = 0x00; tmpB = 0x00;
	State = SM_Init;	
	
    	/* Insert your solution below */
    	while (1) 
	{
		tmpA = PINA & 0x87;	
		PORTB = SM();
    	}
    	return 1;
}

