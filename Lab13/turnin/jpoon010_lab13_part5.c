/*	Author: lab
 *  Partner(s) Name: Jim Poon
 *	Lab Section:
 *	Assignment: Lab #13  Exercise #5
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *	Demo Link: 
 *	https://drive.google.com/file/d/1g9CNUVJlv1ew1O-CBg0oRT734AECqXIC/view?usp=sharing
 *
 */
#include <avr/io.h>
#include "timer.h"
//#include "scheduler.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

void A2D_init() 
{
      ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADEN: Enables analog-to-digital conversion
	// ADSC: Starts analog-to-digital conversion
	// ADATE: Enables auto-triggering, allowing for constant
	//	    analog to digital conversions.
}

void Set_A2D_Pin(unsigned char pinNum) 
{
	ADMUX = (pinNum <= 0x07) ? pinNum : ADMUX;
	// Allow channel to stabilize
	static unsigned char i = 0;
	for ( i=0; i<15; i++ ) 
	{
 		asm("nop"); 
	} 
}


unsigned short last, tmpA, freq;

enum States1 {WAIT, HOLD};
unsigned char pattern;
unsigned char row;
unsigned char mode;
unsigned char lineLR, lineUD; 
int Tick1(int state) 
{
	switch (state) {
		case WAIT:
			tmpA = ADC;
			if(mode == 1)
			{
				if (tmpA > (last+40) && lineLR < 8) 
				{ 
					pattern = pattern << 1;
					lineLR++;
					if(tmpA - last < 100)
						freq = 500;
					else if( (tmpA - last)>=100 && (tmpA - last)<200 )
						freq = 250;
					else if( (tmpA - last)>=200 && (tmpA - last)<300 )
						freq = 100;
					else if( (tmpA - last)>=300)
						freq = 30;		
				} 
				else if (tmpA < (last-40) && lineLR > 1) 
				{ 
					pattern = pattern >> 1;
					lineLR--;
					if(last - tmpA < 100)
						freq = 500;
					else if( (last - tmpA)>=100 && (last - tmpA)<200 )
						freq = 250;
					else if( (last - tmpA)>=200 && (last - tmpA)<300 )
						freq = 100;
					else if( (last - tmpA)>=300)
						freq = 30;
				}
			}
			else if(mode == 2)
			{
				if(tmpA > (last+40) && lineUD < 5)
				{
					row = row << 1;
					row = row | 0x01;
					lineUD++;
					if(tmpA - last < 100)
						freq = 500;
					else if( (tmpA - last)>=100 && (tmpA - last)<200 )
						freq = 250;
					else if( (tmpA - last)>=200 && (tmpA - last)<300 )
						freq = 100;
					else if( (tmpA - last)>=300)
						freq = 30;
				}
				else if (tmpA < (last-40) && lineUD > 1)
				{
					row = row >> 1;
					row = row | 0x80;
					lineUD--;
					if(last - tmpA < 100)
						freq = 500;
					else if( (last - tmpA)>=100 && (last - tmpA)<200 )
						freq = 250;
					else if( (last - tmpA)>=200 && (last - tmpA)<300 )
						freq = 100;
					else if( (last - tmpA)>=300)
						freq = 30;
				}
			}
			state = HOLD;
			break;
		case HOLD:
			state = WAIT;
			break;
		default:
			state = WAIT;
			//mode = 1;
			break;
	}
	PORTC = pattern;	// Pattern to display
	PORTD = row;		// Row(s) displaying pattern	
	return state;
}


enum States2 {LR, UD, WAIT2};
int Tick2(int state)
{
	switch(state)
	{
		case LR:				
			state = WAIT2;
			Set_A2D_Pin(0x01);
			break;
		case UD:		
			state = WAIT2;
			Set_A2D_Pin(0x00);
			break;
		case WAIT2:
			if(mode == 1)
			{
								
				state = UD;
				mode = 2;
			}
			else if(mode == 2)
			{
								
				state = LR;
				mode = 1;
			}
			break;
		default:
			state = LR;
			mode = 1;
	}
	return state;
}

int main(void) {
	//DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	
	int state1, state2;
	state1 = 0;
	state2 = 0;
	freq = 1000;
	pattern = 0x01; row = 0xEF;
	last = tmpA = 0x0000;
	lineLR = 1; lineUD = 5;
	mode = 1;
	Set_A2D_Pin(0x00);


	A2D_init();
	TimerSet(1);
	TimerOn();
	
	last = ADC;
    	while (1) 
	{
		TimerSet(freq);
		state1 = Tick1(state1);		
		//TimerSet(100);
		state2 = Tick2(state2);			

		while(!TimerFlag);
		TimerFlag = 0;	
    	}
    	return 1;
}
