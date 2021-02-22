/*	Author: lab
 *  Partner(s) Name: Jim Poon
 *	Lab Section:
 *	Assignment: Lab #11  Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *	Demo Link: 
 *	https://drive.google.com/file/d/1iYbrYufoUb5jVgL9nL6hAn8XBY4vnhnP/view?usp=sharing
 *
 */
#include <avr/io.h>
#include "timer.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

void transmit_data(unsigned char data) {
    int i;
    for (i = 0; i < 8 ; ++i) {
   	 // Sets SRCLR to 1 allowing data to be set
   	 // Also clears SRCLK in preparation of sending data
   	 PORTC = 0x08;
   	 // set SER = next bit of data to be sent.
   	 PORTC |= ((data >> i) & 0x01);
   	 // set SRCLK = 1. Rising edge shifts next bit of data into the shift register
   	 PORTC |= 0x02;  
    }
    // set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
    PORTC |= 0x04;
    // clears all lines in preparation of a new transmission
    PORTC = 0x00;
}

enum States_DL {INIT, WAIT, INPUT, DISPLAY, END} stateDL;
enum States_C1 {INITC1, DISC1} stateC1;
enum States_C2 {INITC2, DISC2} stateC2;
enum States_C3 {INITC3, DISC3} stateC3;
unsigned char A0;
unsigned char A1;
unsigned char data;
unsigned char cycle;

void CYCLE1()
{
	switch(stateC1)
	{
		case INITC1:
			stateC1 = DISC1;
			data = 0x80;
			break;
		case DISC1:
			if(data != 0x01)
				data = data >> 1;
			else if(data == 0x01)
				data = 0x80;
			break;
	}
}

void CYCLE2()
{
	switch(stateC2)
	{
		case INITC2:
			stateC2 = DISC2;
			data = 0x01;
			break;
		case DISC2:
			if(data != 0x80)
				data = data << 1;
			else if(data == 0x80)
				data = 0x01;
			break;
	}
}

unsigned char left;
unsigned char right;
void CYCLE3()
{
	switch(stateC3)
	{
		case INITC3:
			stateC3 = DISC3;
			left = 0x80;
			right = 0x01;
			data = (left | right);
			break;
		case DISC3:
			if(data != 0x18)
			{
				left = left >> 1;
				right = right << 1;
			}
			else if(data == 0x18)
			{
				left = 0x80;
				right = 0x01;
			}
			data = (left | right);
			break;
	}
}

void Tick()
{
	A0 = ~PINA & 0x01;
	A1 = ~PINA & 0x02;
	switch(stateDL)
	{
		case INIT: 
			stateDL = WAIT;
			data = 0x00;
			break;
		case WAIT:	
			if(A0 && A1)
			{
				stateDL = INPUT;
				cycle = 1;
				data = 0x00;
			}
			break;
		case INPUT:
			if(!A0 && !A1)
				stateDL = DISPLAY;
			break;
		case DISPLAY:
			if(A0 && !A1)
			{
				stateDL = INPUT;
				if(cycle < 3)
					cycle++;
			}
			else if(!A0 && A1)
			{
				stateDL = INPUT;
				if(cycle > 1)
					cycle--;
			}
			else if(A0 && A1)
			{
				stateDL = END;
				data = 0x00;
			}
			else
			{
				if(cycle == 1)
					CYCLE1();
				else if(cycle == 2)
					CYCLE2();
				else if(cycle == 3)
					CYCLE3();
			}
			break;
		case END: 
			if(A0 && A1)
				stateDL = END;
			else
				stateDL = WAIT;
			break;
		default:
			stateDL = INIT;
	}
	transmit_data(data);
}

int main(void) {
	DDRA = 0x00; PORTA = 0xFF;
	//DDRB = 0xFF; PORTA = 0x00;
	DDRC = 0xFF; PORTC = 0x00;	
	data = 0x02;
	stateDL = INIT;	

	TimerSet(300);
	TimerOn();

    	while (1) {				
		Tick();		
		while(!TimerFlag);
		TimerFlag = 0;	
    	}
    	return 1;
}
