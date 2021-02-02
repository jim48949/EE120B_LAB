/*	Author: lab
 *  Partner(s) Name: Jim Poon
 *	Lab Section:
 *	Assignment: Lab #6  Exercise #3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *	
 *	Demo Link: 
 *	https://drive.google.com/file/d/13pH2LHndJED1vyYWCgZvCkx-H2Q8Ss4m/view?usp=sharing
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "timer.h"
#endif

enum States{SM_Init, SM_Start, SM_A0, SM_A1, SM_RESET} State;

unsigned char tmpA; // Temporary variable to hold the value of A
unsigned char tmpB; // Temporary variable to hold the value of B
unsigned char counter;
unsigned char Tick()
{		
	switch(State)
	{
		case SM_Init:
			State = SM_Start;
			tmpB = 0x07;
			break;
		case SM_Start:
			if(tmpA == 0x03)
				State = SM_RESET;
			else if(tmpA == 0x01)
			{	
				if(tmpB < 0x09)
				{
					tmpB++;
					State = SM_A0;	
				}
				else
					State = SM_A0;
			}	
			else if(tmpA == 0x02)
			{
				if(tmpB > 0x00)	
				{			
					tmpB--;
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
			{	
				if(counter == 10)
				{				
					if(tmpB < 0x09)
					{
						tmpB++;
						State = SM_A0;	
					}
					else
						State = SM_A0;
					counter = 0;
				}
				else
					counter++; 
			}
			else if( (tmpA|0x02) == 0x03)
			{
				tmpB = 0x00;
				State = SM_RESET;
			}
			else
				State = SM_Start;	
			break;
		case SM_A1:
			if(tmpA == 0x02)
			{
				if(counter == 10)
				{				
					if(tmpB > 0x00)	
					{			
						tmpB--;
						State = SM_A1;
					}
					else
						State = SM_A1;
					counter = 0;
				}
				else
					counter++; 
			}
			else if( (tmpA|0x01) == 0x03)
			{
				tmpB = 0x00;
				State = SM_RESET;
			}
			else
				State = SM_Start;	
			break;
		case SM_RESET:
			tmpB = 0x00;
			State = SM_Start;
			break;
		default:
			State = SM_Init;
	}
	return tmpB;
}

int main(void) {
	DDRA = 0x00; PORTA = 0xFF; // Port A's 8 pins as inputs
	DDRB = 0xFF; PORTB = 0x00; // Port B's 8 pins as outputs
	tmpB = 0x00; tmpA = 0x00;
	counter = 0;	
	State = SM_Init;
	TimerSet(100);
	TimerOn();
	while (1) {
		tmpA = ~PINA & 0x03;	
		PORTB = Tick();
		while(!TimerFlag);
		TimerFlag = 0;
	    }
	return 1;
}
