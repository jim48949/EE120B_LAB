/*	Author: lab
 *  Partner(s) Name: Jim Poon
 *	Lab Section:
 *	Assignment: Lab #12  Exercise #4
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

void transmit_data(unsigned char data) {
    int i;
    for (i = 0; i < 8 ; ++i) {
   	 // Sets SRCLR to 1 allowing data to be set
   	 // Also clears SRCLK in preparation of sending data
   	 PORTD = 0x08;
   	 // set SER = next bit of data to be sent.
   	 PORTD |= ((data >> i) & 0x01);
   	 // set SRCLK = 1. Rising edge shifts next bit of data into the shift register
   	 PORTD |= 0x02;  
    }
    // set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
    PORTD |= 0x04;
    // clears all lines in preparation of a new transmission
    PORTD = 0x00;
}


enum States {WAIT, HOLD, S2};
unsigned char A0 = 0, A1 = 0, A2 = 0, A3 = 0;
unsigned char pattern1, pattern2;
unsigned char row1, row2;
unsigned char line8, line5; 
unsigned char cnt; 
unsigned char is1, is2, is3, is4;

int Tick(int state) 
{
	A0 = ~PINA & 0x01;
	A1 = ~PINA & 0x02;
	A2 = ~PINA & 0x04;
	A3 = ~PINA & 0x08;						

	switch (state) {
		case WAIT:	
			if (A0 && !A1 && !A2 && !A3 && line8 < 5) 
			{ 			
				pattern1 = pattern1 << 1;
				is1 = 1;			
				state = S2;
			} 
			else if (!A0 && A1 && !A2 && !A3 && line8 > 1) 
			{ 				
				pattern1 = pattern1 >> 1;
				is2 = 1;
				state = S2;
			}
			else if(!A0 && !A1 && A2 && !A3 && line5 < 3) 
			{			
				row1 = (row1 << 1) | 0x01;
				is3 = 1;
				state = S2;
			}
			else if(!A0 && !A1 && !A2 && A3 && line5 > 1) 
			{
				row1 = (row1 >> 1) | 0xF0;
				is4 = 1;
				state = S2;
			}
			break;
		case HOLD:
			if(A0 || A1 || A2 || A3)
				state = HOLD;
			else
				state = WAIT;
			break;
		case S2:
			if(is1)
			{
				pattern2 = pattern2 << 1;					
				line8++;
				is1 = 0;
				state = HOLD;
			}
			else if(is2)
			{
				pattern2 = pattern2 >> 1;	
				line8--;
				is2 = 0;
				state = HOLD;
			}
			else if(is3)
			{
				row2 = (row2 << 1) | 0x01;					
				line5++;
				is3 = 0;
				state = HOLD;
			}
			else if(is4)
			{
				row2 = (row2 >> 1) | 0xF0;					
				line5--;
				is4 = 0;
				state = HOLD;
			}
			break;
		default:
			state = WAIT;
			break;
	}
	if(cnt % 2 != 0)
	{
		PORTC = pattern1;
		//PORTD = row1;
		transmit_data(row1);
	}
	else
	{
		PORTC = pattern2;	
		//PORTD = row2;
		transmit_data(row2);
		cnt = 0;
	}
	return state;
}



int main(void) {
	DDRD = 0xFF; PORTD = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRA = 0x00; PORTA = 0xFF;
	cnt = 0; 
	is1 = 0; is2 = 0; is3 = 0; is4 = 0; 
	line8 = 3; line5 = 2;
	pattern1 = 0x3C; pattern2 = 0x24; 
	row1 = 0xF5; row2 = 0x1B;
	int state = 0;
	TimerSet(1);
	TimerOn();
    	while (1) {

		//transmit_data(0x24);
		//PORTC = 0x1B;		
		state = Tick(state);	
		cnt++;
		while(!TimerFlag);
		TimerFlag = 0;	
    	}
    	return 1;
}
