/*	Author: lab
 *  Partner(s) Name: Jim Poon
 *	Lab Section:
 *	Assignment: Lab #9  Exercise #3
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
enum SK_States {SK_Init, SK_ON, SK_OFF} stateSK;
unsigned char SK;
unsigned char A2;
unsigned char isON;

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
			CL = TL | BL | SK;
			break;
		default: 
			stateCL = CL_COMB;
	}
	PORTB = CL;
}

void Speaker()
{
	A2 = ~PINA & 0x04;
	switch(stateSK)
	{
		case SK_Init:
			stateSK = SK_OFF;
			SK = 0x00;
			isON = 0;
			break;
		case SK_ON:
			if(A2 && isON)
			{
				stateSK = SK_OFF;
				SK = 0x00;
				isON = 0;
			}
			else if(!A2)
			{
				stateSK = SK_OFF;
				SK = 0x00;
			}
			break;
		case SK_OFF:
			if(A2)
			{
				stateSK = SK_ON;
				SK = 0x10;
				isON = 1;
			}
			break;
		default: 
			stateSK = SK_Init;
	}
}

int main(void) {
    	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	unsigned long BL_elapsedTime = 0;
	unsigned long TL_elapsedTime = 0;
	unsigned long SK_elapsedTime = 0;
	const unsigned long timerPeriod = 1;
	
	TimerSet(2);
	TimerOn();
	stateTL = TL_Init;
	stateBL = BL_Init;
	stateSK = SK_Init;
    	while (1) {
				
		if(BL_elapsedTime >= 500)
		{
			BlinkLEDs();
			BL_elapsedTime = 0;
		}
		if(TL_elapsedTime >= 150)
		{
			ThreeLEDs();
			TL_elapsedTime = 0;
		}
		if(SK_elapsedTime >= 2)
		{
			Speaker();
			SK_elapsedTime = 0;
		}
		CombineLEDs();
	
		while(!TimerFlag);
		TimerFlag = 0;	
		BL_elapsedTime += timerPeriod;
		TL_elapsedTime += timerPeriod;
		SK_elapsedTime += timerPeriod;
    	}
    	return 1;
}
