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

enum States{Start, RESET, WaitX1, WaitY, WaitX2, LOCK, UNLOCK} state;

unsigned char tmpA1;
unsigned char tmpA2;
unsigned char tmpA3;
unsigned char tmpA4;
unsigned char tmpA7;
unsigned char tmpB;
unsigned char arr[4] = {0x04, 0x01, 0x02, 0x01};

unsigned char Tick()
{
	
	tmpA1 = PINA & arr[0];
	tmpA2 = PINA & arr[1];
	tmpA3 = PINA & arr[2];
	tmpA4 = PINA & arr[3];
	tmpA7 = PINA & 0x80;
		
	switch(state){
		case Start:
			state = RESET;
			break;
		case RESET:
			if(tmpA7 && tmpB)
				state = LOCK;
			else if(tmpA1)
				state = WaitX1;
			else
				state = RESET;
			break;
		case WaitX1:
			if(!tmpA1 && tmpA2)
				state = WaitY;
			else if((!tmpA1 && !tmpA2) || (tmpA1 && !tmpA2))
				state = WaitX1;
			else
				state = RESET;
			break;
		case WaitY:
			if(!tmpA2 && tmpA3)
				state = WaitX2;
			else if((!tmpA2 && !tmpA3) || (tmpA2 && !tmpA3))
				state = WaitY;
			else
				state = RESET;
			break;
		case WaitX2:
			if(!tmpA3 && tmpA4)
				state = UNLOCK;
			else if((!tmpA3 && !tmpA4) || (tmpA3 && !tmpA4))
				state = WaitX2;
			else
				state = RESET;
			break;
		case LOCK:
			state = RESET;
			break;
		case UNLOCK:
			state = RESET;
		default:
			state = Start;
			break;
	}
	switch(state)
	{
		case Start:
			break;
		case WaitX1:
			break;
		case WaitY:
			break;
		case WaitX2:
			break;
		case LOCK:
			tmpB = 0;
			break;
		case RESET:
			break;
		case UNLOCK:
			tmpB = 1;
			break;
		default:
			break;
	}
	return tmpB;
}

int main(void)
{
	/* Replace with your application code */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	tmpA1 = 0x00; tmpA2 = 0x00; tmpA3 = 0x00; 
	tmpA4 = 0x00; tmpA7 = 0x00;
	tmpB = 0x00;
	state = Start;
	
	while (1)
	{
		PORTB = Tick();	
	}
}

