/*	Author: lab
 *  Partner(s) Name: Jim Poon
 *	Lab Section:
 *	Assignment: Lab #10  Exercise #4
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *	Demo Link: 
 *	https://drive.google.com/file/d/17HhEZvGKR_s9QlwwBtZ55pHwwgOH_mCT/view?usp=sharing
 *
 */
#include <avr/io.h>
#include "keypad.h"
#include "scheduler.h"
#include "timer.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

// 0.954 hz is lowest frequency possible with this function,
// based on settings in PWM_on()
// Passing in 0 as the frequency will stop the speaker from generating sound
void set_PWM(double frequency) {
	static double current_frequency;
	if (frequency != current_frequency) {
		if (!frequency) { TCCR3B &= 0x08; } //stops timer/counter
		else { TCCR3B |= 0x03; } // resumes/continues timer/counter
		
		// prevents OCR3A from overflowing, using prescaler 64
		// 0.954 is smallest frequency that will not result in overflow
		if (frequency < 0.954) { OCR3A = 0xFFFF; }
		
		// prevents OCR0A from underflowing, using prescaler 64					
		// 31250 is largest frequency that will not result in underflow
		else if (frequency > 31250) { OCR3A = 0x0000; }
		
		// set OCR3A based on desired frequency
		else { OCR3A = (short)(8000000 / (128 * frequency)) - 1; }

		TCNT0 = 0; // resets counter
		current_frequency = frequency; // Updates the current frequency
	}
}

void PWM_on() {
	TCCR3A = (1 << COM3A0);
	// COM3A0: Toggle PB3 on compare match between counter and OCR3A
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	// WGM02: When counter (TCNT0) matches OCR0A, reset counter
	// CS31 & CS30: Set a prescaler of 64
	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

//Shared Variables
unsigned char SM1_output;
unsigned char counter;
unsigned char isLocked;
unsigned char sequence[5] = {'1', '2', '3', '4', '5'};
// SMs
enum SM1_States { SM1_Start, SM1_WAIT, SM1_INPUT, SM1_HOLD, SM1_UNLOCK, SM1_WAITB, SM1_RESET, SM1_NEWCOMB, SM1_HOLDNEW };

int SMTick1(int state) {

	// Local Variables
	unsigned char x = GetKeypadKey();
	unsigned char B7 = ~PINA & 0x01;
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
			else if(B7 && x == '*')
			{
				state = SM1_RESET;
				counter = 0;
				SM1_output = 0x05;
			}
			break;
		case SM1_INPUT:			
			if(counter < 4 && x == sequence[counter])
			{
				counter++;
				state = SM1_HOLD;
				//SM1_output = 0x02;
			}
			else if(counter == 4 && x == sequence[counter])
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
		case SM1_RESET:
			if(B7 || x != '\0')
				state = SM1_RESET;
			else
				state = SM1_NEWCOMB;
			break;
		case SM1_NEWCOMB:
			if(counter < 5 && x != '\0')
			{
				sequence[counter] = x;
				counter++;
				state = SM1_HOLDNEW;
				SM1_output = counter;
			}
			else if(counter < 5 && x == '\0')
			{
				state = SM1_NEWCOMB;
			}
			else if(counter == 5)
			{
				state = SM1_WAIT;
				SM1_output = 0;
			}
			break;
		case SM1_HOLDNEW:
			if(x != '\0')
				state = SM1_HOLDNEW;
			else
				state = SM1_NEWCOMB;
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


enum SM3_States { SM3_START, SM3_WAIT, SM3_PLAY, SM3_HOLD, SM3_BUTTOM};

unsigned char counter; // 15 = 3 sec							10
double music[15] = {0, 329.63, 349.23, 392.00, 349.23, 392.00, 349.23, 329.63, 329.63, 329.63, 349.23, 392.00, 349.23, 392.00, 349.23};
unsigned char wait[15] = {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
unsigned char length[15] = {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
unsigned char done;
unsigned char number;

int SMTick3(int state) 
{
	unsigned char A7 = ~PINA & 0x80;
	switch (state) 
	{
		case SM3_START:
			state = SM3_WAIT;
			break;
		case SM3_WAIT:
			if(A7)
			{
				state = SM3_PLAY;
				counter = 0;
				done = 0;
				number = 0;
				PWM_on();
			}	
			break;
		case SM3_PLAY:	
			if(!A7 && done)
			{
				state = SM3_WAIT;
				PWM_off();
			}
			else if(A7 && done)
			{
				state = SM3_BUTTOM;
				PWM_off();
			}
			else if(!done && length[counter] == number)
			{
				number = 0x00;
				set_PWM(0);
				state = SM3_HOLD;
			}
			else 
			{
				number++;
				state = SM3_PLAY;
			}
			break;
		case SM3_HOLD :	
			if(wait[counter] == number)
			{
				number = 0x00;
				counter++;
				if(counter < 6){
					set_PWM(music[counter]);
				}
				else{
					done = 0x01;
				}
				state = SM3_PLAY;
			}
			else
			{
				number++;
				state = SM3_HOLD;
			}
			break;
		case SM3_BUTTOM :
			if(A7)
				state = SM3_BUTTOM;
			else
				state = SM3_WAIT;
			break;
		default:		
			state = SM3_START;
			break;
	}

	return state;
}



int main(void) {
		
	// Set Data Direction Registers
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xF0; PORTC = 0x0F;
	SM1_output = 0x00;
	isLocked = 0;
	unsigned short i;

	// Period for the tasks
	unsigned long int SMTick1_peri = 50;
	unsigned long int SMTick2_peri = 10;
	unsigned long int SMTick3_peri = 200;

	unsigned long int GCD = 1;
	GCD = findGCD(SMTick1_peri, SMTick2_peri);
	GCD = findGCD(GCD, SMTick3_peri);
	SMTick1_peri = SMTick1_peri/GCD;
	SMTick2_peri = SMTick2_peri/GCD;
	SMTick3_peri = SMTick3_peri/GCD;

	//Declare an array of tasks 
	static task task1, task2, task3;
	task *tasks[] = { &task1, &task2, &task3 };
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	// Tasks
	task1.state = 0;
	task1.period = SMTick1_peri;
	task1.elapsedTime = SMTick1_peri;
	task1.TickFct = &SMTick1;

	task2.state = 0;
	task2.period = SMTick2_peri;
	task2.elapsedTime = SMTick2_peri; 
	task2.TickFct = &SMTick2; 
    	
	task3.state = 0;
	task3.period = SMTick3_peri;
	task3.elapsedTime = SMTick3_peri; 
	task3.TickFct = &SMTick3; 

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
