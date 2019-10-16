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
const unsigned long tasksPeriod = 50;

// task structure
#define NUM_TASKS 2

#define LED PORTA

unsigned char data = 0;
unsigned char tFlag = 1;
unsigned char uCSelect = 1;

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
                SPI_MasterTransmit(data, uCSelect - 1);
            }
            else
            {
                data = receivedData;
            }
        }
    }

    return state;
}

enum ValueStates
{
    INIT,
    UPDATE
} valueState;

int valueSM(int state)
{
    static unsigned char speed = 1;
    static unsigned char pattern = 1;
    static unsigned char keypadKey = 0;

    const unsigned char patCursor = 6;
    const unsigned char spdCursor = 12;

    switch(state)
    {
        case INIT:
        {
            if(tFlag) 
            {
                LCD_ClearScreen();
                LCD_DisplayString(1, "Ptrn:");
                LCD_Cursor(patCursor);
                LCD_WriteData(pattern + '0');
                LCD_DisplayString(8, "Spd:");
                LCD_Cursor(spdCursor);
                LCD_WriteData(speed + '0');
                LCD_DisplayString(17, "uC:");
                LCD_Cursor(21);
                LCD_WriteData(uCSelect + '0');

                data = (pattern << 4) | speed;
            }
            else
            {
                LED = data;
            }

            state = UPDATE;

            break;
        }
        case UPDATE:
        {
            if(tFlag)
            {
                if(GetKeypadKey() != '\0' && GetKeypadKey() != keypadKey) 
                {
                    keypadKey = GetKeypadKey();
                }
                switch(keypadKey)
                {
                    case '1': speed = 1; break;
                    case '2': speed = 2; break;
                    case '3': speed = 3; break;
                    case '4': speed = 4; break;
                    case '5': speed = 5; break;
                    case '6': speed = 6; break;
                    case '7': uCSelect = 1; break; 
                    case '8': uCSelect = 2; break;
                    case '9': uCSelect = 3; break;
                    case 'A': pattern = 1; break;
                    case 'B': pattern = 2; break;
                    case 'C': pattern = 3; break;
                    case 'D': pattern = 4; break;
                    default: break;
                }
            }
            else
            {
//                LED = value; 
            }

            state = UPDATE; 

            break;
        }
    }

    // actions
    switch(state)
    {
        case INIT: break;
        case UPDATE:
        {
            if(tFlag) 
            {
                LCD_Cursor(patCursor);
                LCD_WriteData(pattern + '0');
                LCD_Cursor(spdCursor);
                LCD_WriteData(speed + '0');
                LCD_Cursor(21);
                LCD_WriteData(uCSelect + '0');
                
                data = (pattern << 4) | speed;
            }
            else
            {
                LED = data; 
            }

            break;
        }
    }

    return state;
}

Task spiTask;
Task valueTask;
Task *tasks[NUM_TASKS] = {
    &spiTask,
    &valueTask,
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
    DDRA = 0xF0; PORTA = 0x0F;
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00;

    LCD_init();

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

    valueTask.state = INIT;
    valueTask.period = 50;
    valueTask.elapsedTime = valueTask.period;
    valueTask.TickFct = &valueSM;
    
    while (1)
    {
    }

    return 0;
}

