/*	Author: Joseph DiCarlantonio
 *  Partner(s) Name: Brandon Tran
 *	Lab Section:
 *	Assignment: Lab 11  Exercise example
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
/*
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
                value = SPDR;
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
                PORTA = value; 
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
*/
void TimerISR()
{
    /*
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
    */
    TimerFlag = 1;
}

int main(void) 
{ 
    //    DDRB = 0xB0; PORTB = 0x4F;
    //DDRB = 0x40; PORTB = 0xBF;
    DDRA = 0xFF; PORTA = 0x00;
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00;

    TimerSet(tasksPeriod);
    TimerOn();

    SPI_MasterInit();
//    SPI_ServantInit();
/*
    tasks[0]->state = SPI;
    tasks[0]->period = 1000;
    tasks[0]->elapsedTime = spiTask.period;
    tasks[0]->TickFct = &spiSM;

    valueTask.state = INCREMENT;
    valueTask.period = 1000;
    valueTask.elapsedTime = valueTask.period;
    valueTask.TickFct = &valueSM;
*/
    unsigned char var = 0;

    while (1)
    {
        var = ~var;
        SPI_MasterTransmit(var);

//        LED = spdrValue; 
        while(!TimerFlag);
        TimerFlag = 0;

    }

    return 0;
}

