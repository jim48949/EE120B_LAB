/*	Author: lab
 *  	Partner(s) Name: Jim Poon
 *	Lab Section:
 *	Assignment: Lab #14  Exercise Advancement 1
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
#include "scheduler.h"
#include "stdlib.h"
#include "time.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif



void transmit_data(unsigned char data) {

    int i;

    for (i = 0; i < 8 ; ++i) {

   	 // Sets SRCLR to 1 allowing data to be set

   	 // Also clears SRCLK in preparation of sending data

   	 PORTC = 0x08;

   	 // set SER = next bit of data to be sent.

   	 PORTC |= ((data >> i) & 0x01);

   	 // set SRCLK = 1. Rising edge shifts next bit of data into the shift register

   	 PORTC |= 0x02;  

    }

    // set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register

    PORTC |= 0x04;

    // clears all lines in preparation of a new transmission

    PORTC = 0x00;

}



unsigned char findX(unsigned char PL)

{

	unsigned char value = 0;	

	if(PL == 1)

	{

		value = 0xFE;

	}

	else if(PL == 2)

	{

		value = 0xFD;

	}

	else if(PL == 3)

	{

		value = 0xFB;

	}

	return value;

}



// Share vars

unsigned char pattern;

unsigned char row;

unsigned char P1X, P1Y;

unsigned char P2X, P2Y;

unsigned char BX, BY;

unsigned char Bline, Brow;

unsigned char CLeft, CRight;

unsigned char P1Score, P2Score;
unsigned char P1Moving, P2Moving, CSPD, SPIN;
unsigned char newRound;

unsigned char direc;
unsigned char P1WIN, P2WIN;



enum P1States {P1INIT, P1WAIT, P1HOLD};

unsigned char A0 = 0, A1 = 0;

unsigned char P1line;  

int P1(int state) 

{

	A0 = ~PINA & 0x01;

	A1 = ~PINA & 0x02;

					

	switch (state) {

		case P1INIT:

			P1Y = 0x01; P1X = 0xF1;
			P1line = 2;
			P1Moving = 0;
			state = P1WAIT;

			break;

		case P1WAIT:	

			P1Y= 0x01;

			if (A0 && !A1 && P1line < 3) 

			{ 

				P1X = (P1X << 1) | 0x01;
				P1line++;
				P1Moving = 1;
				//state = P1HOLD;

			} 

			else if (!A0 && A1 && P1line > 1) 

			{ 

				P1X = (P1X >> 1) | 0x80;
				P1line--;
				P1Moving = 1;
				//state = P1HOLD;

			} 
			else
			{
				P1Moving = 0;
			}


			if(newRound)

			{

				state = P1INIT;

			}

			break;

		case P1HOLD:

			if(A0 || A1)

				state = P1HOLD;

			else

				state = P1WAIT;

			break;

		default:

			state = P1INIT;

	}

        return state;

}





enum BOTStates {BOTINIT, BOTGUESS};

unsigned char P2line;

unsigned randNum;

int BOT(int state)

{

	switch(state)

	{

		case BOTINIT:

			P2Y = 0x80; P2X = 0xF1;
			P2line = 2;
			P2Moving = 0;
			state = BOTGUESS;

			break;

		case BOTGUESS:

			if(Brow == 6 && direc)

			{

				if(randNum > 3) // 60%

				{
					P2Moving = 0;
					if(P2line < BX && P2line > 1)
					{
						P2X = (P2X >> 1) | 0x80;
						P2line--;
						P2Moving = 1;
					}
					else if(P2line >= BX && P2line < 3)
					{
						P2X = (P2X << 1) | 0x01;
						P2line++;
						P2Moving = 1;
					}
				}
				else
				{
					P2Moving = 0;
				}
			}

			

			if(newRound)

			{

				state = BOTINIT;

			}

			break;

		default:

			state = BOTINIT;

	}

	return state;	

}





enum BStates {BINIT, BUPDATE, BEND}BState;

unsigned char CP1[3], CP2[3];

unsigned char SCORED;
unsigned char A3;



int Ball(int state)

{

	CP1[0] = findX(P1line);

	CP1[1] = (CP1[0] << 1) | 0x01;

	CP1[2] = (CP1[1] << 1) | 0x01;

	CP2[0] = findX(P2line);	

	CP2[1] = (CP2[0] << 1) | 0x01;

	CP2[2] = (CP2[1] << 1) | 0x01;
	A3 = ~PINA & 0x08;

	

	switch(state)

	{

		case BINIT:

			BX = 0xFB;
			BY = 0x02;
			direc = 1; // UP
			CLeft = 0; CRight = 0;
			Bline = 3; Brow = 2;
			SCORED = 0;
			newRound = 0;
			CSPD = 0;
			SPIN = 0;
			state = BUPDATE;

			break;

		case BUPDATE:

			if(direc) // UP

			{

				if(CLeft && !CRight)

				{

					if(Brow < 7)

					{

						if(Bline < 5)

						{

							BX = (BX << 1) | 0x01;
							Bline++;

							if(SPIN && Bline < 5)
							{
								BX = (BX << 1) | 0x01;
								Bline++;
							}
							BY = BY << 1;
							Brow++;
							CLeft = 1;
							CRight = 0;

						}						

						else if(Bline == 5)

						{

							CLeft = 0;

							CRight = 1;

							BX = (BX >> 1) | 0x80;

							Bline--;

							BY = BY << 1;

							Brow++;

						}

					}

					else if(Brow == 7)

					{						

						if(BX == CP2[0])

						{

							// Hit Right
							
							if(Bline > 1) // Go Right

							{

								BX = (BX >> 1) | 0x80;

								Bline--;

								CLeft = 0;

								CRight = 1;

							}

							else if(Bline == 1) // Bounce Left

							{

								BX = (BX << 1) | 0x01;

								Bline++;

								CLeft = 1;

								CRight = 0;

							}

							SCORED = 0;
							CSPD = 1;
							if(P2Moving)
							{
								SPIN = 1;
							}
							else
							{
								SPIN = 0;
							}
						}

						else if(BX == CP2[1])

						{

							// Hit Center
							if(!CLeft && !CRight) // Go Straight
							{
								CLeft = 0;
								CRight = 0;
							}
							else if(CLeft && !CRight) // Go left
							{
								BX = (BX << 1) | 0x01;
								Bline++;
								CLeft = 1;
								CRight = 0;
							}
							else if(!CLeft && CRight) // Go Right
							{
								BX = (BX >> 1) | 0x80;
								Bline--;
								CLeft = 0;
								CRight = 1;
							}

							SCORED = 0;
							CSPD = 0;
							if(P2Moving)
							{
								SPIN = 1;
								CSPD = 1;
							}
							else
							{
								SPIN = 0;
							}
						}

						else if(BX == CP2[2])

						{

							// Hit Left

							if(Bline < 5) // Go Left

							{

								BX = (BX << 1) | 0x01;

								Bline++;

								CLeft = 1;

								CRight = 0;

							}

							else if(Bline == 5) // Bounce Right

							{

								BX = (BX >> 1) | 0x80;

								Bline--;

								CLeft = 0;

								CRight = 1;

							}

							SCORED = 0;
							CSPD = 1;
							if(P2Moving)
							{
								SPIN = 1;
							}
							else
							{
								SPIN = 0;
							}
						}

						else

						{

							SCORED = 1;

						}



						if(!SCORED)

						{

							direc = 0;

							BY = BY >> 1;

							Brow--;

						}

						else

						{

							state = BINIT;

							P1Score++;

							newRound = 1;

						}

					}

				}

				else if(CRight && !CLeft)

				{

					if(Brow < 7)

					{

						if(Bline > 1)

						{

							BX = (BX >> 1) | 0x80;
							Bline--;

							if(SPIN && Bline < 5)
							{
								BX = (BX >> 1) | 0x80;
								Bline--;
							}
							CLeft = 0;
							CRight = 1;

						}						

						else if(Bline == 1)

						{

							CLeft = 1;

							CRight = 0;

							BX = (BX << 1) | 0x01;

							Bline++;	

						}

						BY = BY << 1;

						Brow++;

					}

					else if(Brow == 7)

					{

						if(BX == CP2[0])

						{

							// Hit Right

							if(Bline > 1) // Go Right

							{

								BX = (BX >> 1) | 0x80;

								Bline--;

								CLeft = 0;

								CRight = 1;

							}

							else if(Bline == 1) // Bounce Left

							{

								CLeft = 1;

								CRight = 0;

								BX = (BX << 1) | 0x01;

								Bline++;

							}

							SCORED = 0;	
							CSPD = 1;
							if(P2Moving)
							{
								SPIN = 1;
							}
							else
							{
								SPIN = 0;
							}
						}

						else if(BX == CP2[1])

						{

							// Hit Center
							if(!CLeft && !CRight) // Go Straight
							{
								CLeft = 0;
								CRight = 0;
							}
							else if(CLeft && !CRight) // Go left
							{
								BX = (BX << 1) | 0x01;
								Bline++;
								CLeft = 1;
								CRight = 0;
							}
							else if(!CLeft && CRight) // Go Right
							{
								BX = (BX >> 1) | 0x80;
								Bline--;
								CLeft = 0;
								CRight = 1;
							}

							SCORED = 0;
							CSPD = 0;
							if(P2Moving)
							{
								SPIN = 1;
								CSPD = 1;
							}
							else
							{
								SPIN = 0;
							}
						}

						else if(BX == CP2[2])

						{

							// Hit Left

							if(Bline < 5) // Go left

							{

								BX = (BX << 1) | 0x01; 

								Bline++;

								CLeft = 1;

								CRight = 0;

							}

							else if(Bline == 5) // Bounce Right

							{

								BX = (BX >> 1) | 0x80;

								Bline--;

								CLeft = 0;

								CRight = 1;

							}

							SCORED = 0;
							CSPD = 1;
							if(P2Moving)
							{
								SPIN = 1;
							}
							else
							{
								SPIN = 0;
							}
						}

						else

						{

							SCORED = 1;

						}



						if(!SCORED)

						{

							direc = 0;

							BY = BY >> 1;

							Brow--;

						}

						else

						{

							state = BINIT;

							P1Score++;

							newRound = 1;

						}

					}

				}

				else if(!CLeft && !CRight)

				{

					if(Brow < 7)

					{

						BY = BY << 1;

						Brow++;

						CLeft = 0;

						CRight = 0;

					}

					else if(Brow == 7)

					{

						if(BX == CP2[0])

						{

							// Hit Right

							if(Bline > 1) // Go Right

							{

								BX = (BX >> 1) | 0x80;

								Bline--;

								CLeft = 0;

								CRight = 1;

							}

							else if(Bline == 1) // Bounce Left

							{

								CLeft = 1;

								CRight = 0;

								BX = (BX << 1) | 0x01;

								Bline++;

							}

							SCORED = 0;	
							CSPD = 1;
							if(P2Moving)
							{
								SPIN = 1;
							}
							else
							{
								SPIN = 0;
							}
						}

						else if(BX == CP2[1])

						{

							// Hit Center
							if(!CLeft && !CRight) // Go Straight
							{
								CLeft = 0;
								CRight = 0;
							}
							else if(CLeft && !CRight) // Go left
							{
								BX = (BX << 1) | 0x01;
								Bline++;
								CLeft = 1;
								CRight = 0;
							}
							else if(!CLeft && CRight) // Go Right
							{
								BX = (BX >> 1) | 0x80;
								Bline--;
								CLeft = 0;
								CRight = 1;
							}	
							SCORED = 0;
							CSPD = 0;
							if(P2Moving && !CLeft && !CRight)
							{
								CSPD = 1;
							}
							else if(P2Moving && (CLeft || CRight))
							{
								SPIN = 1;
								CSPD = 1;
							}
						}

						else if(BX == CP2[2])

						{

							// Hit Left

							if(Bline < 5) // Go left

							{

								BX = (BX << 1) | 0x01; 

								Bline++;

								CLeft = 1;

								CRight = 0;

							}

							else if(Bline == 5) // Bounce Right

							{

								BX = (BX >> 1) | 0x80;

								Bline--;

								CLeft = 0;

								CRight = 1;

							}

							SCORED = 0;
							CSPD = 1;
							if(P2Moving)
							{
								SPIN = 1;
							}
							else
							{
								SPIN = 0;
							}
						}
						else

						{

							SCORED = 1;

						}



						if(!SCORED)

						{

							direc = 0;

							BY = BY >> 1;

							Brow--;

						}
						else

						{

							state = BINIT;

							P1Score++;

							newRound = 1;

						}

					}

				}

			}

			else if(!direc) // DOWN

			{

				if(CLeft && !CRight)

				{

					if(Brow > 2)

					{

						if(Bline < 5)

						{

							BX = (BX << 1) | 0x01;
							Bline++;

							if(SPIN && Bline < 5)
							{
								BX = (BX << 1) | 0x01;
								Bline++;
							}
							BY = BY >> 1;
							Brow--;
							CLeft = 1;
							CRight = 0;

						}						

						else if(Bline == 5)

						{

							CLeft = 0;

							CRight = 1;

							BX = (BX >> 1) | 0x80;

							Bline--;

							BY = BY >> 1;

							Brow--;

						}

					}

					else if(Brow == 2)

					{						

						if(BX == CP1[0])

						{

							// Hit Right

							if(Bline > 1) // Go Right

							{

								BX = (BX >> 1) | 0x80;

								Bline--;

								CLeft = 0;

								CRight = 1;

							}

							else if(Bline == 1) // Bounce Left

							{

								BX = (BX << 1) | 0x01;

								Bline++;

								CLeft = 1;

								CRight = 0;

							}

							SCORED = 0;
							CSPD = 1;
							if(P1Moving)
							{
								SPIN = 1;
							}
							else
							{
								SPIN = 0;
							}
						}

						else if(BX == CP1[1])

						{

							// Hit Center
							if(!CLeft && !CRight) // Go Straight
							{
								CLeft = 0;
								CRight = 0;
							}
							else if(CLeft && !CRight) // Go left
							{
								BX = (BX << 1) | 0x01;
								Bline++;
								CLeft = 1;
								CRight = 0;
							}
							else if(!CLeft && CRight) // Go Right
							{
								BX = (BX >> 1) | 0x80;
								Bline--;
								CLeft = 0;
								CRight = 1;
							}

							SCORED = 0;
							CSPD = 0;
							if(P1Moving)
							{
								SPIN = 1;
								CSPD = 1;
							}
							else
							{
								SPIN = 0;
							}
						}

						else if(BX == CP1[2])

						{

							// Hit Left

							if(Bline < 5) // Go Left

							{

								BX = (BX << 1) | 0x01;

								Bline++;

								CLeft = 1;

								CRight = 0;

							}

							else if(Bline == 5) // Bounce Right

							{

								BX = (BX >> 1) | 0x80;

								Bline--;

								CLeft = 0;

								CRight = 1;

							}

							SCORED = 0;
							CSPD = 1;
							if(P1Moving)
							{
								SPIN = 1;
							}
							else
							{
								SPIN = 0;
							}
						}

						else

						{

							SCORED = 1;

						}

						if(!SCORED)

						{

							direc = 1;

							BY = BY << 1;

							Brow++;

						}

						else

						{

							state = BINIT;

							P2Score++;

							newRound = 1;

						}

					}

				}

				else if(CRight && !CLeft)

				{

					if(Brow > 2)

					{

						if(Bline > 1)

						{

							BX = (BX >> 1) | 0x80;
							Bline--;

							if(SPIN && Bline < 5)
							{
								BX = (BX >> 1) | 0x80;
								Bline--;
							}
							CLeft = 0;
							CRight = 1;

						}						

						else if(Bline == 1)

						{

							CLeft = 1;

							CRight = 0;

							BX = (BX << 1) | 0x01;

							Bline++;	

						}

						BY = BY >> 1;

						Brow--;

					}

					else if(Brow == 2)

					{

						if(BX == CP1[0])

						{

							// Hit Right

							if(Bline > 1) // Go Right

							{

								BX = (BX >> 1) | 0x80;

								Bline--;

								CLeft = 0;

								CRight = 1;

							}

							else if(Bline == 1) // Bounce Left

							{

								CLeft = 1;

								CRight = 0;

								BX = (BX << 1) | 0x01;

								Bline++;

							}

							SCORED = 0;	
							CSPD = 1;
							if(P1Moving)
							{
								SPIN = 1;
							}
							else
							{
								SPIN = 0;
							}
						}

						else if(BX == CP1[1])

						{

							// Hit Center
							if(!CLeft && !CRight) // Go Straight
							{
								CLeft = 0;
								CRight = 0;
							}
							else if(CLeft && !CRight) // Go left
							{
								BX = (BX << 1) | 0x01;
								Bline++;
								CLeft = 1;
								CRight = 0;
							}
							else if(!CLeft && CRight) // Go Right
							{
								BX = (BX >> 1) | 0x80;
								Bline--;
								CLeft = 0;
								CRight = 1;
							}

							SCORED = 0;
							CSPD = 0;
							if(P1Moving)
							{
								SPIN = 1;
								CSPD = 1;
							}
							else
							{
								SPIN = 0;
							}
						}

						else if(BX == CP1[2])

						{

							// Hit Left

							if(Bline < 5) // Go left

							{

								BX = (BX << 1) | 0x01; 

								Bline++;

								CLeft = 1;

								CRight = 0;

							}

							else if(Bline == 5) // Bounce Right

							{

								BX = (BX >> 1) | 0x80;

								Bline--;

								CLeft = 0;

								CRight = 1;

							}

							SCORED = 0;
							CSPD = 1;
							if(P1Moving)
							{
								SPIN = 1;
							}
							else
							{
								SPIN = 0;
							}
						}

						else

						{

							SCORED = 1;

						}



						if(!SCORED)

						{

							direc = 1;

							BY = BY << 1;

							Brow++;

						}

						else

						{

							state = BINIT;

							P2Score++;

							newRound = 1;

						}

					}

				}

				else if(!CLeft && !CRight)

				{

					if(Brow > 2)

					{

						BY = BY >> 1;
						Brow--;

					}

					else if(Brow == 2)

					{

						if(BX == CP1[0])

						{

							// Hit Right

							if(Bline > 1) // Go Right

							{

								BX = (BX >> 1) | 0x80;

								Bline--;

								CLeft = 0;

								CRight = 1;

							}

							else if(Bline == 1) // Bounce Left

							{

								CLeft = 1;

								CRight = 0;

								BX = (BX << 1) | 0x01;

								Bline++;

							}

							SCORED = 0;	
							CSPD = 1;
							if(P1Moving)
							{
								SPIN = 1;
							}
							else
							{
								SPIN = 0;
							}
						}

						else if(BX == CP1[1])

						{

							// Hit Center
							if(!CLeft && !CRight) // Go Straight
							{
								CLeft = 0;
								CRight = 0;
							}
							else if(CLeft && !CRight) // Go left
							{
								BX = (BX << 1) | 0x01;
								Bline++;
								CLeft = 1;
								CRight = 0;
							}
							else if(!CLeft && CRight) // Go Right
							{
								BX = (BX >> 1) | 0x80;
								Bline--;
								CLeft = 0;
								CRight = 1;
							}

							SCORED = 0;
							CSPD = 0;
							if(P1Moving && !CLeft && !CRight)
							{
								CSPD = 1;
							}
							else if(P1Moving && (CLeft || CRight))
							{
								CSPD = 1;
								SPIN = 1;
							}
						}

						else if(BX == CP1[2])

						{

							// Hit Left

							if(Bline < 5) // Go left

							{

								BX = (BX << 1) | 0x01; 

								Bline++;

								CLeft = 1;

								CRight = 0;

							}

							else if(Bline == 5) // Bounce Right

							{

								BX = (BX >> 1) | 0x80;

								Bline--;

								CLeft = 0;

								CRight = 1;

							}

							SCORED = 0;
							CSPD = 1;
							if(P1Moving)
							{
								SPIN = 1;
							}
							else
							{
								SPIN = 0;
							}
						}

						else

						{

							SCORED = 1;

						}



						if(!SCORED)

						{

							direc = 1;

							BY = BY << 1;

							Brow++;

						}

						else

						{

							state = BINIT;

							P2Score++;

							newRound = 1;

						}

					}				

				}	

			}
			
			if(P1WIN || P2WIN)
			{
				state = BEND;
			}

			break;
		case BEND:
			if(A3) // Reset
			{
				newRound = 1;
				P1Score = 0;
				P2Score = 0;
				P1WIN = 0;
				P2WIN = 0;
				state = BINIT;
			}
			else
			{
				newRound = 0;
				state = BEND;
			}
			break;

		default:

			state = BINIT;

	}

	return state;

}



enum FStates {OUT1, OUT2, OUT3, WIN, FLASH};

unsigned char outputX, outputY;

unsigned char tmpB, i;

int Output(int state)

{

	switch(state)

	{

		case OUT1:

			outputY = P1Y;

			outputX = P1X;

			state = OUT2;		

			break;

		case OUT2:

			outputY = P2Y;

			outputX = P2X;

			state = OUT3;		

			break;

		case OUT3:

			outputY = BY;

			outputX = BX;

			state = OUT1;
			if(P1Score == 3 && P2Score < 3)
			{
				P1WIN = 1;
				P2WIN = 0;
				i = 0;
				state = WIN;
			}
			else if(P1Score < 3 && P2Score == 3)
			{
				P2WIN = 1;
				P1WIN = 0;
				i = 0;
				state = WIN;
			}		

			break;
		case WIN:
			if(P1WIN)
			{
				if(i < 100)
				{
					i++;
					outputY = 0x01;
					outputX = 0xE0;
				}
				else
				{
					i = 0;
					state = FLASH;
				}
			}
			else if(P2WIN)
			{
				if(i < 100)
				{
					i++;
					outputY = 0x80;
					outputX = 0xE0;
				}
				else
				{
					i = 0;
					state = FLASH;
				}
			}
			else if(!P1WIN && !P2WIN && newRound)
			{
				state = OUT1;
			}
			break;
		case FLASH:
			if(i < 100)
				{
					i++;
					outputY = 0x00;
					outputX = 0xE0;
				}
				else
				{
					i = 0;
					state = WIN;
				}
			break;

		default:

			state = OUT1;

	}

	transmit_data(outputY);

	PORTD = outputX;

	tmpB = P1Score;

	tmpB |= (P2Score << 2);

	PORTB = tmpB;

	return state;

}



int main(void) {

	DDRD = 0xFF; PORTD = 0x00;

	DDRC = 0xFF; PORTC = 0x00;

	DDRB = 0xFF; PORTB = 0x00;

	DDRA = 0x00; PORTA = 0xFF;	

	P1line = P2line = 2;

	P1Y = 0x01; P1X = 0xF1;

	P2Y = 0x80; P2X = 0xF1;

	BX = 0xFB; BY = 0x02;
	P1WIN = P2WIN = 0;
	CSPD = 0; SPIN = 0;

	srand(time(0));



	P1Score = P2Score = 0;

	

	unsigned short i;

	// Period for the tasks

	unsigned long int P1_peri = 100;

	unsigned long int BALL_peri = 150;
	unsigned long int BOT_peri = 100;

	unsigned long int OUT_peri = 1;



	unsigned long int GCD = 1;

	P1_peri = P1_peri/GCD;

	BALL_peri = BALL_peri/GCD;

	OUT_peri = OUT_peri/GCD;
	BOT_peri = BOT_peri/GCD;



	//Declare an array of tasks 

	static task task1, task2, task3, task4;

	task *tasks[] = { &task1, &task2, &task3, &task4 };

	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);



	// Tasks

	task1.state = -1;

	task1.period = P1_peri;

	task1.elapsedTime = P1_peri;

	task1.TickFct = &P1;


	task2.state = -1;

	task2.period = BALL_peri;

	task2.elapsedTime = BALL_peri; 

	task2.TickFct = &Ball; 


	task3.state = -1;

	task3.period = BOT_peri;

	task3.elapsedTime = BOT_peri; 

	task3.TickFct = &BOT; 


	task4.state = -1;

	task4.period = OUT_peri;

	task4.elapsedTime = OUT_peri; 

	task4.TickFct = &Output; 

    	



	TimerSet(GCD);

	TimerOn();



    	while (1) {

		randNum = rand() % 10;
		if(CSPD)
		{
			task2.period = 100;
			task2.elapsedTime = 100; 
		}
		else if(!CSPD)
		{
			task2.period = BALL_peri; // 150
			task2.elapsedTime = BALL_peri; // 150
		}	

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
