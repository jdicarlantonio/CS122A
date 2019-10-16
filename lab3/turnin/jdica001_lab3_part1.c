/*	Author: Joseph DiCarlantonio
 *  Partner(s) Name: Raymond Chlebeck
 *	Lab Section:
 *	Assignment: Lab 3  Exercise 1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

// user includes
#include "../header/keypad.h"
#include "../header/time.h"
#include "../header/io.h"
#include "../header/bit.h"
#include "../header/spi.h"
#include "../header/scheduler.h"

// timer globals
const unsigned long tasksPeriod = 1000;

// task structure
#define NUM_TASKS 2

#define LED PORTA

unsigned char value = 0;
unsigned char tFlag = 1;

enum SpiStates
{
    SPI 
} spiState;

int spiSM(int state)
{
    switch(state) 
    {
        case SPI:
        {
            if(tFlag) 
            {
                SPI_MasterTransmit(value);
            }
            else
            {
                value = spdrValue;
            }
        }
    }

    return state;
}

enum ValueStates
{
    INCREMENT
} valueState;

int valueSM(int state)
{
    switch(state)
    {
        case INCREMENT:
        {
            if(tFlag)
            {
                value++; 
            }
            else
            {
                LED = value; 
            }
        }
    }

    return state;
}

Task spiTask;
Task valueTask;
Task *tasks[NUM_TASKS] = {
    &spiTask,
    &valueTask
};

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
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00;

    TimerSet(tasksPeriod);
    TimerOn();

    if(tFlag)
    {
        SPI_MasterInit();
    }
    else
    {
        SPI_ServantInit();
    }

    spiTask.state = SPI;
    spiTask.period = 1000;
    spiTask.elapsedTime = spiTask.period;
    spiTask.TickFct = &spiSM;

    valueTask.state = INCREMENT;
    valueTask.period = 1000;
    valueTask.elapsedTime = valueTask.period;
    valueTask.TickFct = &valueSM;

    while (1)
    {
    }

    return 0;
}

