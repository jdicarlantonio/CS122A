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

void ADC_init()
{
    ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
}

int main(void) 
{
    DDRC = 0xFF; PORTC = 0x00;

    ADC_init();
    SPI_ServantInit();
//    SPI_MasterInit();

    unsigned char newLightValue = 0;
    while (1) 
    {
        newLightValue = ADC; 
    
        SPDR = newLightValue;
    }
    return 1;
}
