/*	Author: lab
 *  Partner(s) Name: Jim Poon
 *	Lab Section:
 *	Assignment: Lab #11  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *	Demo Link: 
 *	https://drive.google.com/file/d/1t8xvhhCDWxZvq-BXrgAMOJ4XWse9Eviy/view?usp=sharing
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

enum States {INIT, WAIT, INPUT, INCREASE, DECREASE} state;
unsigned char A0;
unsigned char A1;
unsigned char data;
void Tick()
{
	A0 = ~PINA & 0x01;
	A1 = ~PINA & 0x02;
	switch(state)
	{
		case INIT: 
			state = WAIT;
			data = 0x00;
			break;
		case WAIT:
			if(A0 && !A1)
			{
				state = INPUT;
				if(data < 0xFF)
					data++;
			}
			else if(!A0 && A1)
			{
				state = INPUT;
				if(data > 0x00)
					data--;
			}
			else if(A0 && A1)
			{
				state = INPUT;
				data = 0x00;
			}
			break;
		case INPUT:
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
	//DDRB = 0xFF; PORTA = 0x00;
	DDRC = 0xFF; PORTC = 0x00;	
	data = 0x02;
	state = INIT;	

	TimerSet(300);
	TimerOn();

    	while (1) {				
		Tick();		
		while(!TimerFlag);
		TimerFlag = 0;	
    	}
    	return 1;
}
