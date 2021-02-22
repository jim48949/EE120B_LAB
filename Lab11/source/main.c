/*	Author: lab
 *  Partner(s) Name: Jim Poon
 *	Lab Section:
 *	Assignment: Lab #11  Exercise #4
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *	Demo Link: 
 *	
 *
 */
#include <avr/io.h>
#include "timer.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

void transmit_data(unsigned short data) {
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
    // set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
    PORTC |= 0x04;

    for (i = 8; i < 16 ; ++i) {
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
    // set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
    PORTC |= 0x04;
    // clears all lines in preparation of a new transmission
    PORTC = 0x00;
}

enum States {INIT, WAIT, INPUT, INCREASE, DECREASE, WAIT2} state;
unsigned char A0;
unsigned char A1;
unsigned short data;
void Tick()
{
	A0 = ~PINA & 0x01;
	A1 = ~PINA & 0x02;
	switch(state)
	{
		case INIT: 
			state = WAIT;
			data = 0x02FF;
			break;
		case WAIT:
			if(A0 && !A1)
			{
				state = INPUT;
				if(data < 0xFFFF)
					data++;
			}
			else if(!A0 && A1)
			{
				state = INPUT;
				if(data > 0x0000)
					data--;
			}
			else if(A0 && A1)
			{
				state = INPUT;
				data = 0x02FF;
			}
			break;
		case INPUT:
			if(!A0 && !A1)
				state = WAIT2;
			break;
		case WAIT2:
			if(!A0 && !A1)
				state = WAIT;
			break;
		default:
			state = INIT;
	}
	transmit_data(data);
}

int main(void) {
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;	
	data = 0x0000;
	state = INIT;	

	TimerSet(100);
	TimerOn();

    	while (1) {				
		Tick();		
		while(!TimerFlag);
		TimerFlag = 0;	
    	}
    	return 1;
}
