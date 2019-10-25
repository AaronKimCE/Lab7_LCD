/*	Author: akim106
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab 7  Exercise 1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "io.h"
#endif

enum Cnt_States{Wait, Press_1, Press_2, Reset, Held_1, Held_2} Cnt_State; //Enumerating States

volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0
unsigned char B1; //Button 1 PA0
unsigned char B2; //Button 2 PA1
unsigned char Output; //Saved value to update to PORTC
const long sec1 = 100;
long count = 0;

// Internal variables mapping AVR's ISR to our clean ISR
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn() {
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff() {
	TCCR1B = 0x00;
}

void TimerISR() {
	TimerFlag = 1;
}

//C programmer will use our ISR rather than this one
ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

//Sets TimerISR() to tick every M ms
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

void TickFct_Cnt() {
    B1 = ~PINA & 0x01; //Getting input from PINA
    B2 = ~PINA & 0x02;

    switch(Cnt_State) { //State transitions
      case Wait: //Initial State: wait for input
        if (B1 && !B2) {
          Cnt_State = Press_1;
        } else if (!B1 && B2) {
          Cnt_State = Press_2;
        } else if (B1 && B2) {
          Cnt_State = Reset;
        } else {
          Cnt_State = Wait;
        }
        break;
      case Press_1: //B1 is pressed
        if (B1 && !B2) {
          Cnt_State = Held_1;
		  count = 0;
        } else if (B1 && B2) {
          Cnt_State = Reset;
        } else {
          Cnt_State = Wait;
        }
        break;
      case Press_2: //B2 is pressed
        if (!B1 && B2) {
          Cnt_State = Held_2;
		  count = 0;
        } else if (B1 && B2) {
          Cnt_State = Reset;
        } else {
          Cnt_State = Wait;
        }
        break;
      case Reset: //B1 & B2 is held
        if (B1 && B2) {
          Cnt_State = Reset;
        } else {
          Cnt_State = Wait;
        }
      case Held_1:
	    if ((B1 && !B2) && count < sec1) {
			Cnt_State = Held_1;
		} else if ((B1 && !B2) && count >= sec1) {
			Cnt_State = Press_1;
			count = 0;
		} else if (B1 && B2) {
			Cnt_State = Reset;
			count = 0;
		} else {
			Cnt_State = Wait;
			count = 0;
		}
      case Held_2:
      if ((!B1 && B2) && count < sec1) {
	      Cnt_State = Held_2;
	      } else if ((!B1 && B2) && count >= sec1) {
	      Cnt_State = Press_2;
	      count = 0;
	      } else if (B1 && B2) {
	      Cnt_State = Reset;
	      count = 0;
	      } else {
	      Cnt_State = Wait;
	      count = 0;
      }	
	
    }

    switch(Cnt_State) { //State actions
      case Wait:
        break;
      case Held_1:
	    ++count;
        break;
      case Press_1: //One B1 press increments by 1
        if (Output < 9) {
          ++Output;
        }
        break;
      case Held_2:
	    ++count;
        break;
      case Press_2: //One B2 press decrements by 1
        if (Output > 0) {
          --Output;
        }
        break;
      case Reset: //Pressing B1 & B2 resets output to 0
        Output = 0x00;
        break; 
    }
}

int main(void) {
    DDRA = 0x00; PORTA = 0xFF; //PORT A = inputs
    DDRC = 0xFF; PORTC = 0x00; //PORT C = outputs (LCD)
	DDRB = 0xFF; PORTD = 0x00; //PORT D = outputs (LCD)
    Cnt_State = Wait; //Setting initial state
    Output = 0;
    TimerSet(10);
	TimerOn();
	
    while (1) {
      TickFct_Cnt(); //Repeating state logic 
      LCD_WriteData(Output + '0');//Updating LCD output
	  while (!TimerFlag);
	  TimerFlag = 0;
    }
    return 1;
}
