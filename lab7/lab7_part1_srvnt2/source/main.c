/*	Author: Joseph DiCarlantonio
 *  Partner(s) Name: Raymond Chlebeck
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

#include "../header/spi.h"
#include "../header/io.h"

unsigned char lightValue = 0;
unsigned char average = 0;

int main(void) 
{
    DDRD = 0xFF; PORTD = 0x00;
    DDRC = 0xFF; PORTC = 0x00;

    SPI_ServantInit(); 
    LCD_init();

    LCD_ClearScreen();

    unsigned char hundred = 0;
    unsigned char ten = 0;
    unsigned char one = 0;

    while (1) 
    {
        average = spdrValue; 

        hundred = average / 100;
        ten = average / 10;
        ten %= 10;
        one = average % 10;

        LCD_Cursor(1);
        LCD_WriteData(hundred + '0');
        LCD_Cursor(2);
        LCD_WriteData(ten + '0');
        LCD_Cursor(3);
        LCD_WriteData(one + '0');
    }

    return 1;
}
