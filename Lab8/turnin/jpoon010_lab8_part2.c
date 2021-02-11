/*	Author: lab
 *  Partner(s) Name: Jim Poon
 *	Lab Section:
 *	Assignment: Lab #8  Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *	Demo Link: 
 *	https://drive.google.com/file/d/1Ze1UKv4nHkN-Hs24VPQTlhF1tyf98YXg/view?usp=sharing
 *
 */
#include <avr/io.h>
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

enum States {Init, OFF, ON, WAIT, OFF_WAIT, ON_WAIT}state;

double notes[8] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};

unsigned char A0;
unsigned char A1;
unsigned char A2;
unsigned char count;

void Tick()
{
	A0 = ~PINA & 0x01;
	A1 = ~PINA & 0x02;
	A2 = ~PINA & 0x04;
	switch(state){
		case Init:
			state = OFF;
			count = 0;
			break;	
		case OFF:
			set_PWM(0.00);			
			if(A0)
				state = ON_WAIT;
			break;
		case ON_WAIT:
			if(A0)
				state = ON_WAIT;
			else
				state = ON;
		case ON:
			if(!A0 && !A1 && !A2)
			{	
				state = ON;
				set_PWM(notes[count]);
			}
			else if(A0 && !A1 && !A2)
			{
				state = OFF_WAIT;
				set_PWM(0.00);
			}
			else if(!A0 && A1 && !A2)
			{
				state = WAIT;
				if(count < 7)
					count++;
				set_PWM(notes[count]);
			}
			else if(!A0 && !A1 && A2)
			{
				state = WAIT;
				if(count > 0)
					count--;
				set_PWM(notes[count]);
			}
			break;
		case WAIT:
			if((!A0 && A1 && !A2) || (!A0 && !A1 && A2))
			{
				state = WAIT;
				set_PWM(notes[count]);
			}
			else
				state = ON;
			break;
		case OFF_WAIT:
			if(A0)
				state = OFF_WAIT;
			else
				state = OFF;
		default: state = Init;
	}
}

int main(void) {
    	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	
	PWM_on();
    	while (1) {	
		Tick();	
    	}
    	return 1;
}
