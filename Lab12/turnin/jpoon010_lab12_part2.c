/*	Author: lab
 *  Partner(s) Name: Jim Poon
 *	Lab Section:
 *	Assignment: Lab #12  Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *	Demo Link: 
 *	https://drive.google.com/file/d/1g2Niz2UvwWyx1PV4DLJxVeLU_eDml01f/view?usp=sharing
 *
 */
#include <avr/io.h>
#include "timer.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

//--------------------------------------
// LED Matrix SynchSM
// Period: 100 ms
//--------------------------------------
enum States {WAIT, HOLD};
unsigned char A0 = 0, A1 = 0;
unsigned char pattern;
unsigned char row;
unsigned char line;  
int Tick(int state) 
{
	A0 = ~PINA & 0x01;
	A1 = ~PINA & 0x02;						

	switch (state) {
		case WAIT:	
			if (A0 && !A1 && line < 8) 
			{ 
				pattern = pattern << 1;
				line++;
				state = HOLD;
			} 
			else if (!A0 && A1 && line > 1) 
			{ 
				pattern = pattern >> 1;
				line--;
				state = HOLD;
			} 
			break;
		case HOLD:
			if(A0 || A1)
				state = HOLD;
			else
				state = WAIT;
			break;
		default:
			state = WAIT;
			break;
	}
	PORTC = pattern;	// Pattern to display
	PORTD = row;		// Row(s) displaying pattern	
	return state;
}



int main(void) {
	DDRD = 0xFF; PORTD = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRA = 0x00; PORTA = 0xFF;	
	int state = 0;
	line = 1;
	pattern = 0x01; row = 0x00;

	TimerSet(100);
	TimerOn();
    	while (1) {				
		
		state = Tick(state);
		while(!TimerFlag);
		TimerFlag = 0;	
    	}
    	return 1;
}
