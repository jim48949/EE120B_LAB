/*	Author: lab
 *  Partner(s) Name: Jim Poon
 *	Lab Section:
 *	Assignment: Lab #13  Exercise #3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *	Demo Link: 
 *	https://drive.google.com/file/d/1HqclMN0REzyHH41qbLQ0-V2mAZjdpjkp/view?usp=sharing
 *
 */
#include <avr/io.h>
#include "timer.h"
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

unsigned short last, tmpA;

enum States {WAIT};
unsigned char pattern;
unsigned char row;
unsigned char line; 
int Tick(int state) 
{
	switch (state) {
		case WAIT:
			if (tmpA > (last+40) && line < 8) 
			{ 
				pattern = pattern << 1;
				line++;
				if(tmpA - last < 150)
					TimerSet(1000);
				else if( (tmpA - last)>=150 && (tmpA - last)<300 )
					TimerSet(500);
				else if( (tmpA - last)>=300 && (tmpA - last)<450 )
					TimerSet(250);
				else if( (tmpA - last)>=450)
					TimerSet(100);
			} 
			else if (tmpA < (last-40) && line > 1) 
			{ 
				pattern = pattern >> 1;
				line--;
				if(last - tmpA < 150)
					TimerSet(1000);
				else if( (last - tmpA)>=150 && (last - tmpA)<300 )
					TimerSet(500);
				else if( (last - tmpA)>=300 && (last - tmpA)<450 )
					TimerSet(250);
				else if( (last - tmpA)>=450)
					TimerSet(100);
			}
			else if(tmpA > (last+40) && line == 8)
			{
				pattern = 0x01;
				line = 1;
				if(tmpA - last < 150)
					TimerSet(1000);
				else if( (tmpA - last)>=150 && (tmpA - last)<300 )
					TimerSet(500);
				else if( (tmpA - last)>=300 && (tmpA - last)<450 )
					TimerSet(250);
				else if( (tmpA - last)>=450)
					TimerSet(100);
			}
			else if(tmpA < (last-40) && line == 1)
			{
				pattern = 0x80;
				line = 8;
				if(last - tmpA < 150)
					TimerSet(1000);
				else if( (last - tmpA)>=150 && (last - tmpA)<300 )
					TimerSet(500);
				else if( (last - tmpA)>=300 && (last - tmpA)<450 )
					TimerSet(250);
				else if( (last - tmpA)>=450)
					TimerSet(100);
			} 
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

	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;

	int state = 0;
	pattern = 0x01; row = 0xEF;
	last = tmpA = 0x0000;
	line = 1;

	A2D_init();
	TimerSet(1000);
	TimerOn();
	
	last = ADC;
    	while (1) 
	{
		tmpA = ADC;
		state = Tick(state);
		while(!TimerFlag);
		TimerFlag = 0;	
    	}
    	return 1;
}
