/*	Author: lab
 *  Partner(s) Name: Jim Poon
 *	Lab Section:
 *	Assignment: Lab #10  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *	Demo Link: 
 *	https://drive.google.com/file/d/1DPjPz4edPuDYkRKaM-Z3PxbEj8Da8LY5/view?usp=sharing
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
unsigned char SM1_output = 0x00;


// SMs
enum SM1_States { SM1_Start, SM1_Input };

// Monitors button connected to PA0. 
// When button is pressed, shared variable "pause" is toggled.
int SMTick1(int state) {

	// Local Variables
	unsigned char x = 0;

	//State machine transitions
	switch (state) 
	{
		case SM1_Start:
			state = SM1_Input;
			break;
		case SM1_Input:
			state = SM1_Input;
			break;
		default:
			state = SM1_Start;
	}

	//State machine actions
	switch(state) 
	{
		case SM1_Start:
			break;
		case SM1_Input:
			x = GetKeypadKey();
			switch (x) 
			{
				case '1': SM1_output = 0x80; break; 
				case '2': SM1_output = 0x80; break;
				case '3': SM1_output = 0x80; break;
				case '4': SM1_output = 0x80; break;
				case '5': SM1_output = 0x80; break;
				case '6': SM1_output = 0x80; break;
				case '7': SM1_output = 0x80; break;
				case '8': SM1_output = 0x80; break;
				case '9': SM1_output = 0x80; break;
				case 'A': SM1_output = 0x80; break;
				case 'B': SM1_output = 0x80; break;
				case 'C': SM1_output = 0x80; break;
				case 'D': SM1_output = 0x80; break;
				case '*': SM1_output = 0x80; break;
				case '0': SM1_output = 0x80; break;
				case '#': SM1_output = 0x80; break;
				default: SM1_output = 0x00; break; 
			}
			break;
		default:
			break;
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
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xF0; PORTC = 0x0F;

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
