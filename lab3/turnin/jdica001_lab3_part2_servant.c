/*	Author: Joseph DiCarlantonio
 *  Partner(s) Name: Raymond Chlebeck
 *	Lab Section:
 *	Assignment: Lab 3  Exercise 3 (servant)
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
const unsigned long tasksPeriod = 50;

// task structure
#define NUM_TASKS 6

#define LED PORTA

unsigned char receivedData = 0;

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
            receivedData = spdrValue;
        }
    }

    return state;
}

// pattern SMs
unsigned char speed = 1;
unsigned char pattern = 1;
unsigned char output1, output2, output3, output4;

// SM for handling which pattern and speed to display on LED bank
enum displayStates
{
    UPDATE
} displayState;

int displaySM(int state)
{
    switch(state)
    {
        case UPDATE:
        {
            pattern = (receivedData & 0xF0) >> 4;
            speed = receivedData & 0x0F;

            switch(speed)
            {
                case 1: speed = 40; break;
                case 2: speed = 20; break;
                case 3: speed = 10; break;
                case 4: speed = 5; break;
                case 5: speed = 2; break;
                case 6: speed = 1; break;
            }

            switch(pattern)
            {
                case 1: LED = output1; break;
                case 2: LED = output2; break;
                case 3: LED = output3; break;
                case 4: LED = output4; break;
            }
        }
    }

    return state;
}

enum PatOneStates
{
    SEQ1,
    SEQ2
} patOneState;

int patOneSM(int state)
{
    static unsigned char count = 0;;
    // transitions
    switch(state)
    {
        case SEQ1:
        {
            if(count < speed)
            {
                state = SEQ1; 
            }
            else
            {
                state = SEQ2; 
                count = 0;
            }

            break;
        }
        case SEQ2:
        {
            if(count < speed)
            {
                state = SEQ2; 
            }
            else
            {
                state = SEQ1; 
                count = 0;
            }

            break;
        }
    }

    // actions
    switch(state)
    {
        case SEQ1: count++; output1 = 0xF0; break;
        case SEQ2: count++; output1 = 0x0F; break;
    }

    return state;
}

enum PatTwoStates
{
    SEQ3,
    SEQ4
} patTwoState;

int patTwoSM(int state)
{
    static unsigned char count = 0;;
    // transitions
    switch(state)
    {
        case SEQ3:
        {
            if(count < speed)
            {
                state = SEQ1; 
            }
            else
            {
                state = SEQ2; 
                count = 0;
            }

            break;
        }
        case SEQ4:
        {
            if(count < speed)
            {
                state = SEQ2; 
            }
            else
            {
                state = SEQ1; 
                count = 0;
            }

            break;
        }
    }

    // actions
    switch(state)
    {
        case SEQ3: count++; output2 = 0xAA; break;
        case SEQ4: count++; output2 = 0x55; break;
    }

    return state;
}

enum PatThreeStates
{
    WAIT,
    SHIFT_RIGHT,
    SHIFT_LEFT
} patThreeState;

int patThreeSM(int state)
{
    static unsigned char value = 0x80;
    static unsigned char count = 0;
    static unsigned char shiftFlag = 1;

    // transitions
    switch(state)
    {
        case WAIT:
        {
            if(count < speed) 
            {
                state = WAIT; 
            }
            else
            {
                if(shiftFlag) 
                {
                    state = SHIFT_RIGHT; 
                }
                else
                {
                    state = SHIFT_LEFT; 
                }

                count = 0;
            }

            break;
        }
        case SHIFT_RIGHT:
        {
            value >>= 1;
            if(value == 0x01)
            {
                shiftFlag = 0; 
            }

            state = WAIT;

            break; 
        }
        case SHIFT_LEFT:
        {
            value <<= 1;
            if(value == 0x80)
            {
                shiftFlag = 1; 
            }

            state = WAIT;

            break; 
        }
    }

    // actions
    switch(state)
    {
        case WAIT: count++; output3 = value; break;
        case SHIFT_RIGHT: break;
        case SHIFT_LEFT: break;
    }

    return state;
}

enum patFourStates
{
    FWAIT,
    FUPDATE
};

int patFourSM(int state)
{
    static unsigned int value = 0x80;
    static unsigned char count = 0;
    static unsigned char temp = 0x80;

    switch(state)
    {
        case FWAIT:
        {
            if(count < speed) 
            {
                state = FWAIT; 
            }
            else
            {
                state = FUPDATE; 
                count = 0;
            }

            break;
        }
        case FUPDATE:
        {
            output4 = value;
            if(value >= 0xFF)
            {
                value = 0x80;
                temp = 0x80;

                state = FWAIT;
                break;
            }

            temp >>= 1;
            value |= temp;

            state = FWAIT;

            break;
        }
    }

    switch(state)
    {
        case FWAIT: count++; break;
        case FUPDATE: break;
    }

    return state;
}

Task spiTask;
Task displayTask;
Task patOneTask;
Task patTwoTask;
Task patThreeTask;
Task patFourTask;
Task *tasks[NUM_TASKS] = {
    &spiTask,
    &displayTask,
    &patOneTask,
    &patTwoTask,
    &patThreeTask,
    &patFourTask
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

    TimerSet(tasksPeriod);
    TimerOn();

    SPI_ServantInit();

    spiTask.state = SPI;
    spiTask.period = 50;
    spiTask.elapsedTime = spiTask.period;
    spiTask.TickFct = &spiSM;
    
    displayTask.state = UPDATE;
    displayTask.period = 50;
    displayTask.elapsedTime = displayTask.period;
    displayTask.TickFct = &displaySM;
    
    patOneTask.state = SEQ1;
    patOneTask.period = 50;
    patOneTask.elapsedTime = patOneTask.period;
    patOneTask.TickFct = &patOneSM;
    
    patTwoTask.state = SEQ3;
    patTwoTask.period = 50;
    patTwoTask.elapsedTime = patTwoTask.period;
    patTwoTask.TickFct = &patTwoSM;

    patThreeTask.state = SHIFT_RIGHT;
    patThreeTask.period = 50;
    patThreeTask.elapsedTime = patThreeTask.period;
    patThreeTask.TickFct = &patThreeSM;
    
    patFourTask.state = FWAIT;
    patFourTask.period = 50;
    patFourTask.elapsedTime = patFourTask.period;
    patFourTask.TickFct = &patFourSM;
    
    while (1)
    {
    }

    return 0;
}

