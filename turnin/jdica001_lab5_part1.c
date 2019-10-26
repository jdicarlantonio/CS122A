/*	Author: Joseph DiCarlantonio
 *  Partner(s) Name: Raymond Chlebeck
 *	Lab Section:
 *	Assignment: Lab 5  Exercise 1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

#include "../header/shiftRegister.h"
#include "../header/time.h"

#define button1 (~PINA & 0x01)
#define button2 (~PINA & 0x02)

// timer globals
const unsigned long period = 100;
const unsigned long changeTime = 5;

// state machine variables
unsigned char value;
unsigned long count;

enum States
{
    WAIT_BUTTON,
    INC,
    DEC,
    RESET
} state;

void tick()
{
    // transitions
    switch(state)
    {
        case WAIT_BUTTON:
        {
            if(!button1 && !button2) 
            {
                state = WAIT_BUTTON; 
            }
            else if(button1 && !button2)
            {
                count = changeTime;
                state = INC;
            }
            else if(button2 && !button1)
            {
                count = changeTime;
                state = DEC; 
            }
            else
            {
                state = RESET; 
            }

            break;
        }
        case INC:
        {
            if(button1 && !button2) 
            {
                state = INC; 
            }
            else if(button2 && !button1)
            {
                state = DEC; 
            }
            else if(!button1 && !button2)
            {
                state = WAIT_BUTTON; 
            }
            else
            {
                state = RESET; 
            }

            break;
        }
        case DEC:
        {
            if(button2 && !button1) 
            {
                state = DEC; 
            }
            else if(!button2 && button1)
            {
                state = INC; 
            }
            else if(!button1 && !button2)
            {
                state = WAIT_BUTTON; 
            }
            else
            {
                state = RESET; 
            }

            break;
        }
        case RESET:
        {
            if(button1 || button2) 
            {
                state = RESET; 
            }
            else
            {
                state = WAIT_BUTTON; 
            }

            break;
        }
    }
    
    // actions
    switch(state)
    {
        case WAIT_BUTTON:
        {
            count = 0; 
            break;
        }
        case INC:
        {
            if(count >= changeTime && value < 0xFF)
            {
                count = 0;
                value++;
            }
            else if(count < changeTime && value < 0xFF)
            {
                count++;
            }
            else 
            {
                value = value; 
            }

            break;
        }
        case DEC:
        {
            if(count >= changeTime && value > 0x00)
            {
                count = 0;
                value--;
            }
            else if(count < changeTime && value > 0x00)
            {
                count++;
            }
            else 
            {
                value = value; 
            }

            break;
        }
        case RESET:
        {
            value = 0;
            break;
        }
    }
}

int main(void) 
{
    DDRA = 0x00; PORTA = 0xFF;

    TimerSet(period);
    TimerOn();

    shiftInit();
    
    count = 0;
    value = 0xF9;
    while (1) 
    {
        tick();
        shiftWrite(value);

        while(!TimerFlag);
        TimerFlag = 0;
    }

    return 1;
}
