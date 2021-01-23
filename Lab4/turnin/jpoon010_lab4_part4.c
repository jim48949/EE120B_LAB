/*	Author: lab
 *  Partner(s) Name: Jim Poon
 *	Lab Section: 023
 *	Assignment: Lab #4  Exercise #4
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum States{Start, Lock, Wait, Same_Code, Unlock} state;

unsigned char tmpA1;
unsigned char tmpA2;
unsigned char tmpA7;
unsigned char tmpB;

void Tick(){
	tmpA1 = PINA & 0x02;
	tmpA2 = PINA & 0x04;
	tmpA7 = PINA & 0x80;
	
	switch(state)
	{
		case Start:
			state = Lock;
			break;
		case Lock:
			if(tmpA2)
				state = Wait;
			else
				state = Lock;
			break;
		case Wait:
			if(!tmpA2 && tmpA1)
				state = Unlock;
			else if((!tmpA2 && !tmpA1) || (tmpA2 && !tmpA1))
				state = Wait;
			else
				state = Lock;
			break;
		case Unlock:
			if(tmpA7)
				state = Lock;
			else if(tmpA2)
				state = Same_Code;
			else
				state = Unlock;
			break;
		case Same_Code:
			if(!tmpA2 && tmpA1)
				state = Lock;
			else if((!tmpA2 && !tmpA1) || (tmpA2 && !tmpA1))
				state = Same_Code;
			else
				state = Lock;
			break;
		default:
			state = Start;
			break;
	}
	switch(state)
	{
		case Start:
			break;
		case Lock:
			tmpB = 0;
			break;
		case Wait:
			break;
		case Unlock:
			tmpB = 1;
			break;
		case Same_Code:
			break;
		default:
			break;
	}
	PORTB = tmpB;
}

int main(void)
{
	/* Replace with your application code */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	
	tmpA1 = 0x00; tmpA2 = 0x00; tmpA7 = 0x00;
	tmpB = 0x00;
	state = Start;
	
	while (1)
	{
		Tick();
	}
}
