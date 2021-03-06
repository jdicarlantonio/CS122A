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
#include <avr/wdt.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

#include "../header/io.h"
#include "../header/time.h"
#include "../header/incDecSM.h"
#include "../header/scheduler.h"

#define WAKE_BUTTON (~PINA & 0x01)

const unsigned long tasksPeriod = 50;
#define NUM_TASKS 3
#define sec3 60

Task incDecTask;
Task displayTask;
Task *tasks[NUM_TASKS] = {
    &incDecTask,
    &displayTask
};

enum DisplayStates
{
    ACTIVE,
    INACTIVE
} displayState;

int displaySM(int state)
{
    static unsigned char dCount = 0;
    // transitions
    switch(state)
    {
        case ACTIVE:
        {
            if(dCount < sec3)
            {
                state = ACTIVE; 
            }
            else
            {
                LCD_ClearScreen();
                PORTB &= ~(0x01);

                state = INACTIVE; 
                dCount = 0;
                displayTask.active = 0x00;
            
            }

            break;
        }
        case INACTIVE:
        {
            state = ACTIVE;

            break;
        }
    }

    // actions
    switch(state)
    {
        case ACTIVE:
        {
            PORTB |= 0x01;

            dCount++; 
            LCD_Cursor(1);
            LCD_WriteData(count + '0');
           
            
            break;
        }
        case INACTIVE:
        {
            // turn off watchdog
            watchdogOff();        
            break;
        }
    }

    return state;
}

void watchdogOff(void)
{
    cli();
    wdt_reset();
    MCUSR &= ~(1<<WDRF);
    WDTCSR |= (1<<WDCE) | (1<<WDE);
    WDTCSR = 0x00;
}

void WDT_Init(void)
{
    cli();
    wdt_reset();
    WDTCSR |= (1<<WDIE); // enable WD interrupt
    WDTCSR |= (1<<WDCE) | (1<<WDE);
    WDTCSR = (1<<WDE) | (1<<WDP3);
    sei();
}

ISR(WDT_vect)
{
}

void enableInterrupt(void)
{
    SREG = 0x01; // enable global interrupts
    PCICR = 0x01; // enable pin change interrupt
    PCMSK0 = 0x01; // enable pin change interrupt on PCINT0

    sei();
}

ISR(PCINT0_vect)
{
    if(WAKE_BUTTON)
    {
        WDT_Init();
        displayTask.active = 0x01; 
    }
}

void TimerISR()
{
    unsigned char i;
    for(i = 0; i < NUM_TASKS; ++i)
    {   
        if(tasks[i]->active)
        {
            if(tasks[i]->elapsedTime >= tasks[i]->period) 
            {
                tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
                tasks[i]->elapsedTime = 0;
            }
            tasks[i]->elapsedTime += tasksPeriod;
        }
    }
}

int main(void) 
{
    DDRA = 0x00; PORTA = 0xFF;
    DDRD = 0xFF; PORTD = 0x00;
    DDRC = 0xFF; PORTC = 0x00;
    DDRB = 0xFF; PORTB = 0x00;

    enableInterrupt();
    watchdogOff();

    TimerSet(tasksPeriod);
    TimerOn();

    LCD_init();
    
    incDecTask.state = Start;
    incDecTask.period = 50;
    incDecTask.elapsedTime = incDecTask.period;
    incDecTask.TickFct = &IncDec_tick;
    incDecTask.active = 0x01;
    
    displayTask.state = INACTIVE;
    displayTask.period = 50;
    displayTask.elapsedTime = displayTask.period;
    displayTask.TickFct = &displaySM;
    displayTask.active = 0x00;
    
    while (1) 
    {

    }
    return 1;
}
