/*	Author: lab
 *  Partner(s) Name: Jim Poon
 *	Lab Section:
 *	Assignment: Lab #6  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *	
 *	Demo Link: 
 *	https://drive.google.com/file/d/1z7IZC_aVVpEqGf4NMjfbSmVeTibo5XFb/view?usp=sharing
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "timer.h"
#endif

enum States{Start, s0, s1, s2} State;

unsigned char tmpB = 0x00;

void Tick()
{
	switch(State){
		case Start:
			State = s0;
			tmpB = 0x01;
			break;
		case s0:
			State = s1;
			tmpB = 0x02;
			break;
		case s1:
			State = s2;
			tmpB = 0x04;
			break;
		case s2:
			State = s0;
			tmpB = 0x01;
			break;
		default:
			State = Start;
	}
	PORTB = tmpB;
}

int main(void) {
	DDRB = 0xFF;
	PORTB = 0x00;
	TimerSet(1000);
	TimerOn();
	tmpB = 0x00;
	while (1) {
		Tick();
		while(!TimerFlag);
		TimerFlag = 0;
	    }
	return 1;
}
