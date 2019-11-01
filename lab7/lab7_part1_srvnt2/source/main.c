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

/*
ISR(PCINT13_vect)
{
    if(flag)
    {
        average = spdrValue; 
    }
    else
    {
        lightValue = spdrValue; 
    }

    flag = ~flag;
}
*/
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
//        if(flag)
//        {
            average = spdrValue; 
//        }
/*        else
        {
            lightValue = spdrValue; 
        }
*/
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
/*
        hundred = lightValue / 100;
        ten = lightValue / 10;
        ten %= 10;
        one = lightValue % 10;

        LCD_Cursor(20);
        LCD_WriteData(hundred + '0');
        LCD_Cursor(21);
        LCD_WriteData(ten + '0');
        LCD_Cursor(22);
        LCD_WriteData(one + '0');
*/
    }

    return 1;
}
