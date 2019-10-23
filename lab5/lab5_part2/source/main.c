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
//#include "../header/spi.h"
#include "../header/scheduler.h"
#include "../header/shiftRegister.h"

// timer globals
const unsigned long tasksPeriod = 50;

// task structure
#define NUM_TASKS 5

// button functions
#define button1 (~PINA & 0x01)
#define button2 (~PINA & 0x02)

// pattern SMs
unsigned char pattern = 1;

// flag to keep track of when lights are on or off
unsigned char on = 0xFF;

// keep track of which output is displayed
unsigned char output = 1;
unsigned char output1, output2, output3;

// speed of pattern display
unsigned char speed = 10;

enum ButtonStates
{
    BWAIT,
    INC_HELD,
    INC,
    DEC_HELD,
    DEC,
    TOGGLE_HELD,
    TOGGLE
} buttonState;

int buttonSM(int state)
{
    // transitions
    switch(state)
    {
        case BWAIT:
        {
            if(!button1 && !button2) 
            {
                state = BWAIT; 
            }
            else if(button1 && !button2)
            {
                state = INC_HELD; 
            }
            else if(!button1 && button2)
            {
                state = DEC_HELD; 
            }
            else
            {
                state = TOGGLE_HELD; 
            }

            break;
        }
        case INC_HELD:
        {
            if(button1 && !button2) 
            {
                state = INC_HELD; 
            }
            else if(!button1 && !button2)
            {
                state = INC; 
            }
            else if(button1 && button2)
            {
                state = TOGGLE_HELD; 
            }

            break;
        }
        case INC:
        {
            output++;
            if(output >= 3)
            {
                output = 3; 
            }

            state = BWAIT;

            break;
        }
        case DEC_HELD:
        {
            if(!button1 && button2) 
            {
                state = DEC_HELD; 
            }
            else if(!button1 && !button2)
            {
                state = DEC; 
            }
            else if(button1 && button2)
            {
                state = TOGGLE_HELD;
            }
            
            break;
        }
        case DEC:
        {
            output--;
            if(output <= 1)
            {
                output = 1; 
            }

            state = BWAIT;

            break;
        }
        case TOGGLE_HELD:
        {
            if(!button1 && !button2)
            {
                state = TOGGLE; 
            }
            else
            {
                state = TOGGLE_HELD; 
            }
            
            break;
        }
        case TOGGLE:
        {
            on = ~on;
            /*
            if(on > 0)
            {
                on = 0x00; 
            }
            else
            {
                on = 0xFF;
            }
            */
            state = BWAIT;

            break;
        }
    }

    return state;
}

enum OutputStates
{
    OUTPUT,
    NO_OUTPUT
} outputState;

int outputSM(int state)
{
    // transitions
    switch(state)
    {
        case OUTPUT:
        {
            if(on) 
            {
                state = OUTPUT; 
            }
            else
            {
                state = NO_OUTPUT; 
            }

            break;
        }
        case NO_OUTPUT:
        {
            if(on) 
            {
                state = OUTPUT; 
            }
            else
            {
                state = NO_OUTPUT; 
            }

            break;
        }
    }

    switch(state) 
    {
        case OUTPUT:
        {
            switch(output) 
            {
                case 1: shiftWrite(output1); break;
                case 2: shiftWrite(output2); break;
                case 3: shiftWrite(output3); break;
            }

            break;
        }
        case NO_OUTPUT: shiftWrite(0x00); break;
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

Task buttonTask;
Task outputTask;
Task patOneTask;
Task patTwoTask;
Task patThreeTask;
Task *tasks[NUM_TASKS] = {
    &patOneTask,
    &patTwoTask,
    &patThreeTask,
    &buttonTask,
    &outputTask
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
    DDRA = 0x00; PORTA = 0xFF;

    TimerSet(tasksPeriod);
    TimerOn();

    shiftInit();

    buttonTask.state = WAIT;
    buttonTask.period = 50;
    buttonTask.elapsedTime = buttonTask.period;
    buttonTask.TickFct = &buttonSM;

    outputTask.state = OUTPUT;
    outputTask.period = 50;
    outputTask.elapsedTime = outputTask.period;
    outputTask.TickFct = &outputSM;

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
    
    while (1)
    {
    }

    return 0;
}

