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
#include "io.h"

int main(void) {
    DDRC = 0xFF; PORTC = 0x00; //PORT C = outputs (LCD)
    DDRD = 0xFF; PORTD = 0x00; //PORT D = outputs (LCD)
    LCD_init();
	
    while (1) {
      LCD_DisplayString(1, "Hello World");
    }
    return 1;
}
