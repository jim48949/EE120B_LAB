/*	Author: lab
 *  Partner(s) Name: Jim Poon
 *	Lab Section:
 *	Assignment: Lab #8  Exercise #3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *	Demo Link: 
 *	https://drive.google.com/file/d/1s2a0hxcoNI6G01Dehvi6eIjCIv4IcVFs/view?usp=sharing
 *
 */
#include <avr/io.h>
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

enum States {Init, OFF, PLAY, WAIT} state;
double music[30] = {0, 0, 261.63, 261.63, 293.66, 261.63, 349.23, 329.63, 261.63, 261.63, 293.66, 261.63, 392.00, 349.23, 261.63, 261.63, 523.25, 440.00, 349.23, 349.23, 329.63, 293.66, 466.00, 466.00, 440.00, 349.23, 392.00, 349.23, 0, 0 };
unsigned char wait[30] = {0, 0, 0x02, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02, 0x01, 0x01, 0x02, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02, 0x02, 0, 0};
unsigned char length[30] = {0, 0, 0x01, 0x01, 0x04, 0x04, 0x04, 0x08, 0x01, 0x01, 0x04, 0x04, 0x04, 0x08, 0x01, 0x01, 0x03, 0x03, 0x02, 0x01, 0x04, 0x01, 0x02, 0x02, 0x04, 0x04, 0x08, 0, 0};

unsigned char count = 0x00;
unsigned char A0;
unsigned char done;
unsigned char number;

void Tick()
{
	A0 = ~PINA & 0x01;
	switch(state){
		case Init:
		state = OFF;
		break;
		
		case OFF:
			if(A0){
				done = 0x00;
				count = 0x00;
				number = 0x00;
				PWM_on();
				state = PLAY;
			}
			else {
				state = OFF;
			}
			break;
		
		case PLAY:
			if(!A0 && done == 0x01){
				PWM_off();
				state = OFF;
			}
			else if(A0 && done == 0x01){
				PWM_off();
				state = PLAY;
			}
			else if(!(!A0 && done == 0x01) && length[count] == number){
				number = 0x00;
				set_PWM(0);
				state = WAIT;
			}
			else {
				number++;
				state = PLAY;
			}
			break;
		
		case WAIT:
			if(wait[count] == number){
				number = 0x00;
				count++;
				if(count < 29){
					set_PWM(music[count]);
				}
				else{
					done = 0x01;
				}
				state = PLAY;
			}
			else{
				number++;
				state = WAIT;
			}
			break;
		default: state = Init;
	}
}

int main(void) {
    	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	
	TimerSet(125);
	TimerOn();
	//PWM_on();
    	while (1) {
		
		Tick();	
		while(!TimerFlag);
		TimerFlag = 0;	

    	}
    	return 1;
}
