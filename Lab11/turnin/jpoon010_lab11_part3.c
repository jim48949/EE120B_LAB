/*	Author: lab
 *  Partner(s) Name: Jim Poon
 *	Lab Section:
 *	Assignment: Lab #11  Exercise #3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *	Demo Link: 
 *	https://drive.google.com/file/d/1nn2kExiMcvljVhWiDEjU4KabxP3Lf1la/view?usp=sharing
 *
 */
#include <avr/io.h>
#include "timer.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

void transmit_data(unsigned char data, unsigned char reg) {
		int i;
	    	for (i = 0; i < 8 ; ++i) {
		   	 // Sets SRCLR to 1 allowing data to be set
		   	 // Also clears SRCLK in preparation of sending data
		   	 PORTC = 0x08;
		   	 // set SER = next bit of data to be sent.
		   	 PORTC |= ((data >> i) & 0x01);
		   	 // set SRCLK = 1. Rising edge shifts next bit of data into the shift register
			if(reg == 1)
		   	 	PORTC |= 0x02;  
			else if(reg == 2)
				PORTC |= 0x20;
	    	}
	    	// set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
		if(reg == 1)
	    		PORTC |= 0x04;
		else if(reg == 2)
			PORTC |= 0x40;
	    	// clears all lines in preparation of a new transmission
	    	PORTC = 0x00;
}

enum States_DL {INIT, WAIT, INPUT1, INPUT2, DISPLAY1, END, DISPLAY2, DOUBLE, WAIT2} stateDL;
enum States_C1 {INITC1, DISC1} stateC1;
enum States_C2 {INITC2, DISC2} stateC2;
enum States_C3 {INITC3, DISC3} stateC3;
unsigned char A0;
unsigned char A1;
unsigned char A2;
unsigned char A3;
unsigned char data1, data2;
unsigned char cycle1, cycle2;
unsigned char isSwitch;
unsigned char direc;

unsigned char CYCLE1(unsigned char data)
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
	return data;
}

unsigned char CYCLE2(unsigned char data)
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
	return data;
}

unsigned char left;
unsigned char right;
unsigned char CYCLE3(unsigned char data)
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
	return data;
}

void Tick()
{
	A0 = ~PINA & 0x01;
	A1 = ~PINA & 0x02;
	A2 = ~PINA & 0x04;
	A3 = ~PINA & 0x08;
	switch(stateDL)
	{
		case INIT: 
			stateDL = WAIT;
			data1 = 0x00;
			data2 = 0x00;
			isSwitch = 0;
			direc = 0;
			break;
		case WAIT:
			isSwitch = 0;	
			if(A0 && A1)
			{
				stateDL = INPUT1;
				cycle1 = 1;
				data1 = 0x00;
			}
			else if(A2 && A3)	
			{
				stateDL = INPUT2;
				cycle2 = 1;
				data2 = 0x00;
			}
			break;
		case INPUT1:
			if(!A0 && !A1)
				stateDL = DISPLAY1;
			break;
		case INPUT2:
			if(!A2 && !A3)
				stateDL = DISPLAY2;
			break;
		case DISPLAY1:
			if(A0 && !A1)
			{
				stateDL = INPUT1;
				if(cycle1 < 3)
					cycle1++;
			}
			else if(!A0 && A1)
			{
				stateDL = INPUT1;
				if(cycle1 > 1)
					cycle1--;
			}
			else if(A0 && A1)
			{
				stateDL = END;
				data1 = 0x00;
			}
			else if(A2 && A3)
			{
				stateDL = WAIT2;
				isSwitch = 1;
				cycle2 = 1;
				data2 = 0x00;
			}

				if(cycle1 == 1)
					data1 = CYCLE1(data1);
				else if(cycle1 == 2)
					data1 = CYCLE2(data1);
				else if(cycle1 == 3)
					data1 = CYCLE3(data1);
			
			break;
		case DISPLAY2:
			if(A2 && !A3)
			{
				stateDL = INPUT2;
				if(cycle2 < 3)
					cycle2++;
			}
			else if(!A2 && A3)
			{
				stateDL = INPUT2;
				if(cycle2 > 1)
					cycle2--;
			}
			else if(A2 && A3)
			{
				stateDL = END;
				data2 = 0x00;
			}
			else if(A0 && A1)
			{
				stateDL = WAIT2;
				isSwitch = 1;
				cycle1 = 1;
				data1 = 0x00;
			}
			if(cycle2 == 1)
					data2 = CYCLE1(data2);
				else if(cycle2 == 2)
					data2 = CYCLE2(data2);
				else if(cycle2 == 3)
					data2 = CYCLE3(data2);

			break;
		case WAIT2:
			if(A0 || A1 || A2 || A3)
				stateDL = WAIT2;
			else
				stateDL = DOUBLE;
			break;
		case DOUBLE:
			if(A2 && A3)
			{
				data2 = 0;
				direc = 1;
				stateDL = END;
			}
			else if(A0 && A1)
			{
				data1 = 0;
				direc = 2;
				stateDL = END;
			}
			else if(A2 && !A3)
			{
				stateDL = WAIT2;
				if(cycle2 < 3)
					cycle2++;
			}
			else if(!A2 && A3)
			{
				stateDL = WAIT2;
				if(cycle2 > 1)
					cycle2--;
			}
			else if(A0 && !A1)
			{
				stateDL = WAIT2;
				if(cycle1 < 3)
					cycle1++;
			}
			else if(!A0 && A1)
			{
				stateDL = WAIT2;
				if(cycle1 > 1)
					cycle1--;
			}

				if(cycle1 == 1)
					data1 = CYCLE1(data1);
				else if(cycle1 == 2)
					data1 = CYCLE2(data1);
				else if(cycle1 == 3)
					data1 = CYCLE3(data1);

				if(cycle2 == 1)
					data2 = CYCLE1(data2);
				else if(cycle2 == 2)
					data2 = CYCLE2(data2);
				else if(cycle2 == 3)
					data2 = CYCLE3(data2);
			break;
		case END: 
			if((A0 && A1) || (A2 && A3))
				stateDL = END;
			else if(!isSwitch)
				stateDL = WAIT;
			else if(isSwitch && direc == 1)
			{	
				stateDL = DISPLAY1;
				isSwitch = 0;
			}
			else if(isSwitch && direc == 2)
			{
				stateDL = DISPLAY2;
				isSwitch = 0;
			}
			break;
		default:
			stateDL = INIT;
	}
	transmit_data(data1, 1);
	transmit_data(data2, 2);
}

int main(void) {
	DDRA = 0x00; PORTA = 0xFF;
	DDRD = 0xFF; PORTD = 0x00;
	DDRC = 0xFF; PORTC = 0x00;	
	stateDL = INIT;	

	TimerSet(100);
	TimerOn();

    	while (1) {				
		Tick();		
		while(!TimerFlag);
		TimerFlag = 0;	
    	}
    	return 1;
}
