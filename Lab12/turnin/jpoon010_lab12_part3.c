/*	Author: lab
 *  Partner(s) Name: Jim Poon
 *	Lab Section:
 *	Assignment: Lab #12  Exercise #3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *	Demo Link: 
 *	https://drive.google.com/file/d/1b_vTxcMSSfstW8U7uambZBi7yNLNxav1/view?usp=sharing
 *
 */
#include <avr/io.h>
#include "timer.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif



int main(void) {
	DDRD = 0xFF; PORTD = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	unsigned short time = 0x0000;

	TimerSet(1);
	TimerOn();
    	while (1) {				
		
		if(time % 2 == 0)
		{
			PORTC = 0x3C;
			PORTD = 0xF5;
		}
		else
		{
			PORTC = 0x24;
			PORTD = 0x1B;
		}
		time++;
		while(!TimerFlag);
		TimerFlag = 0;	
    	}
    	return 1;
}
