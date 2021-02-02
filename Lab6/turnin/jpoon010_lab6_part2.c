/*	Author: lab
 *  Partner(s) Name: Jim Poon
 *	Lab Section:
 *	Assignment: Lab #6  Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *	
 *	Demo Link: 
 *	https://drive.google.com/file/d/12Vw8dcTZBeDslAQtYo1k2iTR2wB_zgCM/view?usp=sharing
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "timer.h"
#endif

enum States{Start, s0, s1, s2, s1r, s0_pause, s1_pause, s2_pause, s1r_pause} State;

unsigned char tmpB = 0x00;
unsigned char tmpA;

void Tick()
{
	switch(State){
		case Start:
			State = s0;
			tmpB = 0x01;
			break;
		case s0:
			if(!tmpA)
			{						
				State = s1;
				tmpB = 0x02;
			}
			else if(tmpA)
				State = s0_pause;
			break;
		case s1:
			if(!tmpA)
			{						
				State = s2;
				tmpB = 0x04;
			}
			else if(tmpA)
				State = s1_pause;
			break;
		case s2:
			if(!tmpA)
			{						
				State = s1r;
				tmpB = 0x02;
			}
			else if(tmpA)
				State = s2_pause;
			break;
		case s1r:
			if(!tmpA)
			{						
				State = s0;
				tmpB = 0x01;
			}
			else if(tmpA)
				State = s1r_pause;
			break;
		case s0_pause:
			if(tmpA)
			{
				State = s0;
				tmpB = 0x01;
			}
			else if(!tmpA)
				State = s0_pause;
			break;
		case s1_pause:
			if(tmpA)
			{
				State = s0;
				tmpB = 0x01;
			}
			else if(!tmpA)
				State = s1_pause;
			break;
		case s2_pause:
			if(tmpA)
			{
				State = s0;
				tmpB = 0x01;
			}
			else if(!tmpA)
				State = s2_pause;
			break;
		case s1r_pause:
			if(tmpA)
			{
				State = s0;
				tmpB = 0x01;
			}
			else if(!tmpA)
				State = s1r_pause;
			break;
		default:
			State = Start;
	}
	PORTB = tmpB;
}

int main(void) {
	DDRB = 0xFF; PORTB = 0x00;
	DDRA = 0x00; PORTA = 0xFF;
	TimerSet(300);
	TimerOn();
	tmpB = 0x00;
	while (1) {
		tmpA = ~PINA & 0x01;
		Tick();
		while(!TimerFlag);
		TimerFlag = 0;
	    }
	return 1;
}
