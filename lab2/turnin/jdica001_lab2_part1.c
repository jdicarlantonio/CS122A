/*	Author: Joseph DiCarlantonio
 *  Partner(s) Name: Raymond Chlebeck
 *	Lab Section:
 *	Assignment: Lab 2  Exercise 1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

#include "../header/usart_ATmega1284.h"
#include "../header/time.h"
#include "../header/scheduler.h"

#define LED PORTA

unsigned char tasksPeriod = 25;

unsigned char ledValue = 0;

#define NUM_TASKS 2
Task usartTask;
Task ledTask;
Task *tasks[NUM_TASKS] = {
   &usartTask,
    &ledTask
};

// indicate whether transmitting or receiving
char tFlag = 0;
unsigned char usartPeriod;
unsigned char ledPeriod;

enum USARTStates
{
    WAIT
} usartState;

int usartSM(int state)
{
    // transitions
    switch(state)    
    {
        case WAIT:
        {
            if(tFlag) 
            {
                if(USART_IsSendReady(1)) 
                {
                    USART_Send(ledValue, 1);
                }

            }
            else
            {
                if(USART_HasReceived(0)) 
                { 
                    ledValue = USART_Receive(0);
                }
            }

            break;
        }
    }

    // actions
    switch(state)
    {
        case WAIT: break;
    }

    return state;
}

enum LEDStates
{
    LED_OFF,
    LED_ON
} ledState;

// turn off and on LED and also transmit LED data 
int ledSM(int state)
{
    // transitions
    switch(state)
    {
        case LED_OFF: state = LED_ON; break;
        case LED_ON: state = LED_OFF; break;
        default: state = LED_OFF;
    }

    // actions
    switch(state)
    {
        case LED_OFF:
        {
            if(tFlag)
            {
                ledValue = 0;
            }
            LED = ledValue;

            break;
        }
        case LED_ON:
        {
            if(tFlag)
            {
                ledValue = 1;
            }
            LED = ledValue;

            break;
        }
    }


    return state;
}

void TimerISR()
{
    unsigned char i;
    for(i = 0; i < NUM_TASKS; ++i)
    {
        if(tasks[i]->elapsedTime >= tasks[i]->period) 
        {
            tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
            tasks[i]->elapsedTime = 0;
        }
        tasks[i]->elapsedTime += tasksPeriod;
    }
}

int main(void) 
{
    DDRA = 0xFF; PORTA = 0x00;
    DDRB = 0x00; PORTB = 0xFF;
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00;

    TimerSet(tasksPeriod);
    TimerOn();

    initUSART(0);
    initUSART(1);

    if(tFlag)
    {
        usartPeriod = 50;
        ledPeriod = 1000;
    }
    else
    {
        usartPeriod = 25;
        ledPeriod = 25;
    }

    usartTask.state = WAIT;
    usartTask.period = 25;
    usartTask.elapsedTime = usartTask.period;
    usartTask.TickFct = &usartSM;

    ledTask.state = LED_OFF;
    ledTask.period = 25;
    ledTask.elapsedTime = ledTask.period;
    ledTask.TickFct = &ledSM;

    while (1) 
    {
    }

    return 1;
}
