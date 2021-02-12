/*	Author: lab
 *  Partner(s) Name: Jim Poon
 *	Lab Section:
 *	Assignment: Lab #9  Exercise #1
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


enum TL_States {TL_Init, TL_S1, TL_S2, TL_S3} stateTL;
unsigned char TL;
enum BL_States {BL_Init, BL_OFF, BL_ON} stateBL;
unsigned char BL;
enum CL_States {CL_COMB} stateCL;
unsigned char CL;


void ThreeLEDs()
{
	switch(stateTL)
	{
		case TL_Init:
			stateTL = TL_S1;
			TL = 0x01;
			break;
		case TL_S1:
			stateTL = TL_S2;
			TL = 0x02;
			break;
		case TL_S2:
			stateTL = TL_S3;
			TL = 0x04;
			break;
		case TL_S3:
			stateTL = TL_S1;
			TL = 0x01;
			break;
		default: 
			stateTL = TL_Init;
			TL = 0x00;
	}
}

void BlinkLEDs()
{
	switch(stateBL)
	{
		case BL_Init:
			stateBL = BL_ON;
			BL = 0x08;
			break;
		case BL_OFF:
			stateBL = BL_ON;
			BL = 0x08;
			break;
		case BL_ON:
			stateBL = BL_OFF;
			BL = 0x00;
			break;
		default: 
			stateBL = BL_Init;
			BL = 0x00;
	}
}

void CombineLEDs()
{
	switch(stateCL)
	{
		case CL_COMB:
			stateCL = CL_COMB;
			BlinkLEDs();
			ThreeLEDs();
			CL = TL | BL;
			break;
		default: 
			stateCL = CL_COMB;
			CL = 0x00;
	}
	PORTB = CL;
}

int main(void) {
    	//DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	
	TimerSet(1000);
	TimerOn();
    	while (1) {
		
		CombineLEDs();
	
		while(!TimerFlag);
		TimerFlag = 0;	
    	}
    	return 1;
}
