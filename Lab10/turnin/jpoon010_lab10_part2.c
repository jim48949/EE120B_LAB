/*	Author: lab
 *  Partner(s) Name: Jim Poon
 *	Lab Section:
 *	Assignment: Lab #10  Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *	Demo Link: 
 *	https://drive.google.com/file/d/1XoBwNOOz3RoBZ0_wcXSPNOCdVyK5NzQx/view?usp=sharing
 *
 */
#include <avr/io.h>
#include "keypad.h"
#include "scheduler.h"
#include "timer.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

//Shared Variables
unsigned char SM1_output;
unsigned char counter;
unsigned char isLocked;
// SMs
enum SM1_States { SM1_Start, SM1_WAIT, SM1_INPUT, SM1_HOLD, SM1_UNLOCK, SM1_WAITB};

int SMTick1(int state) {

	// Local Variables
	unsigned char x = GetKeypadKey();
	unsigned char B7 = PINB & 0x08;
	//State machine transitions
	switch (state) 
	{
		case SM1_Start:
			counter = 0;
			isLocked = 0;
			SM1_output = 0x01;
			state = SM1_WAIT;
			break;
		case SM1_WAIT:
			if(x == '#')// && isLocked)
			{
				counter = 0;
				state = SM1_HOLD;
				//SM1_output = 0x0E;
			}
			else if(B7 && !isLocked)
			{
				state = SM1_WAITB;				
				SM1_output = 0x00;
				isLocked = 1;
			}
			break;
		case SM1_INPUT:			
			if(counter == 0 && x == '1')
			{
				counter++;
				state = SM1_HOLD;
				//SM1_output = 0x02;
			}
			else if(counter == 1 && x == '2')
			{
				counter++;
				state = SM1_HOLD;
				//SM1_output = 0x04;
			}
			else if(counter == 2 && x == '3')
			{
				counter++;
				state = SM1_HOLD;
				//SM1_output = 0x08;
			}
			else if(counter == 3 && x == '4')
			{
				counter++;
				state = SM1_HOLD;
				//SM1_output = 0x06;
			}
			else if(counter == 4 && x == '5')
			{
				counter++;
				state = SM1_UNLOCK;
				SM1_output = 0x01;
				isLocked = 0;
			}
			else if(x == '\0')
				state = SM1_INPUT;
			else
			{
				SM1_output = 0x00;
				state = SM1_WAIT;
			}
			break;
		case SM1_HOLD:
			if(x != '\0')
				state = SM1_HOLD;
			else
				state = SM1_INPUT;
			break;
		case SM1_WAITB:
			if(B7)
				state = SM1_WAITB;
			else
				state = SM1_WAIT;
			break;
		case SM1_UNLOCK:
			state = SM1_WAIT;
			break;
		default:
			state = SM1_Start;
	}
	return state;
}

enum SM2_States { SM2_display };


int SMTick2(int state) 
{

	unsigned char output;
	switch (state) 
	{
		case SM2_display:	
			break;

		default:		
			state = SM2_display;
			break;
	}

	switch(state) 
	{
		case SM2_display:	
			output = SM1_output; 
			break;
		default: 
			break;
	}
		
	PORTB = output;
	return state;
}


int main(void) {
		
	// Set Data Direction Registers
	//DDRA = 0x00; PORTB = 0xFF;
	DDRB = 0x0F; PORTB = 0xF0;
	DDRC = 0xF0; PORTC = 0x0F;
	SM1_output = 0;
	isLocked = 0;
	unsigned short i;

	// Period for the tasks
	unsigned long int SMTick1_peri = 50;
	unsigned long int SMTick2_peri = 10;

	unsigned long int GCD = 1;
	GCD = findGCD(SMTick1_peri, SMTick2_peri);
	SMTick1_peri = SMTick1_peri/GCD;
	SMTick2_peri = SMTick2_peri/GCD;

	//Declare an array of tasks 
	static task task1, task2;
	task *tasks[] = { &task1, &task2 };
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	// Tasks
	task1.state = -1;
	task1.period = SMTick1_peri;
	task1.elapsedTime = SMTick1_peri;
	task1.TickFct = &SMTick1;

	task2.state = -1;
	task2.period = SMTick2_peri;
	task2.elapsedTime = SMTick2_peri; 
	task2.TickFct = &SMTick2; 
    	

	TimerSet(GCD);
	TimerOn();

    	while (1) {
				
		for ( i = 0; i < numTasks; i++ ) 
		{
			if ( tasks[i]->elapsedTime == tasks[i]->period ) 
			{
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 1;
		}
		while(!TimerFlag);
		TimerFlag = 0;
    	}
    	return 1;
}
