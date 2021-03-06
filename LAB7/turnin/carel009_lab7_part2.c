/*	Author: Christopher Arellano
 *  Partner(s) Name: 
 *	Lab Section: 026
 *	Assignment: Lab #7  Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

//Demo: https://drive.google.com/open?id=1Yjku13ZhdOEiowsJqf_O2NgETDoGPC8u

#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn()
{
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1=0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}
void TimerOff()
{
	TCCR1B = 0x00;
}
void TimerISR()
{
	TimerFlag = 1;
}
ISR(TIMER1_COMPA_vect)
{
	_avr_timer_cntcurr--;
		if (_avr_timer_cntcurr == 0)
		{
			TimerISR();
			_avr_timer_cntcurr = _avr_timer_M;
		}
}
void TimerSet(unsigned long M)
{
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

enum States {Start, LED1, LED2, LED3, LED_2, CHECK, STOP, RESET} state;
unsigned char cnt = 0;

void Tick()
{
	switch (state)
	{
		case Start:
			cnt = 5;
			LCD_Cursor (1);
			LCD_WriteData (cnt + '0');
			state = LED1;
			break;
		case LED1:
			if ((~PINA & 0x01) == 0x01)
			{
			state = CHECK;
			}
			else
			{
			state = LED2;
			} 
			break;
		case LED2:
			if ((~PINA & 0x01) == 0x01)
			{
			state = CHECK;
			}
			else
			{
			state = LED3;
			} 
			break;
		case LED3:
			if ((~PINA & 0x01) == 0x01)
			{
			state = CHECK;
			}
			else
			{
			state = LED_2;
			}
			break;
		case LED_2:
			if ((~PINA & 0x01) == 0x01)
			{
			state = CHECK;
			}
			else
			{
			state = LED1;
			}
			break;
		case CHECK:
			state = STOP;
			break;
		case STOP:
			if ((~PINA & 0x01) == 0x01)
			{
			state = STOP;
			}
			else
			{
			state = RESET;
			}
			break;
		case RESET:
			if ((~PINA & 0x01) == 0x01)
			{
				if (cnt == 9)
				{
				cnt = 5;
				LCD_ClearScreen ();
				LCD_Cursor (1);
				LCD_WriteData (cnt + '0');
				}
			state = LED1;	
			}
			else
			{
			state = RESET;
			}
			break;
		default:
			break;
	}
	switch (state)
	{
		case Start:
			PORTB = 0x00;
			break;
		case LED1:
			PORTB = 0x01;
			break;
		case LED2:
			PORTB = 0x02;
			break;
		case LED3:
			PORTB = 0x04;
			break;
		case LED_2:
			PORTB = 0x02;
			break;
		case CHECK:
			if (PORTB == 0x02)
			{
			cnt = cnt + 1;
				if (cnt >= 9)
				{
				cnt = 9;
				LCD_DisplayString (1, "You Win!");
				}
				else
				{
				LCD_Cursor (1);
				LCD_WriteData (cnt + '0');
				}
			}
			else
			{
				if (cnt > 0)
				{
				cnt = cnt - 1;
				}
			LCD_Cursor (1);
			LCD_WriteData (cnt + '0');
			}
			break;
		case STOP:
			break;
		case RESET:
			break;
		default:
			break;
	}
}

int main(void)
{
	DDRA = 0x00;	PORTA = 0xFF;
	DDRB = 0xFF;	PORTB = 0x00;
	DDRC = 0xFF;	PORTC = 0x00;
	DDRD = 0xFF;	PORTD = 0x00;
	state = Start;
	LCD_init();
	TimerSet(300);
	TimerOn();
	while(1)
	{
		Tick();
		while (!TimerFlag);
		TimerFlag = 0;
	}
return 0;
}
