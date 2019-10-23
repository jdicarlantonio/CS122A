/*	Author: Joseph DiCarlantonio
 *  Partner(s) Name: Raymond Chlebeck
 *	Lab Section:
 *	Assignment: Lab 4  Exercise 4
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <stdint.h> 
#include <stdlib.h> 
#include <stdio.h> 
#include <stdbool.h> 
#include <string.h> 
#include <math.h> 
#include <avr/io.h> 
#include <avr/interrupt.h> 
#include <avr/eeprom.h> 
#include <avr/portpins.h> 
#include <avr/pgmspace.h> 
 
//FreeRTOS include files 
#include "FreeRTOS.h" 
#include "task.h" 
#include "croutine.h" 

#define LED PORTD
#define button (~PINA & 0x01)
    
unsigned char shiftFlag = 0;

enum ButtonStates
{
    BUTTON_HELD,
    BUTTON_RELEASE
} button_state;

void button_init()
{
    button_state = BUTTON_RELEASE;
}

void button_tick()
{
    switch(button_state)
    {
        case BUTTON_RELEASE:
        {
            if(button) 
            {
                button_state = BUTTON_HELD; 
            }
            else
            {
                button_state = BUTTON_RELEASE; 
            }

            break;
        }
        case BUTTON_HELD:
        {
            if(!button) 
            {
                shiftFlag = 1;
                button_state = BUTTON_RELEASE; 
            }
            else
            {
                button_state = BUTTON_HELD; 
            }

            break;
        }
    }
}

enum LEDState 
{
    SHIFT_RIGHT,
    SHIFT_LEFT
} led_state;


void LEDS_Init()
{
    led_state = SHIFT_RIGHT;
}

void LEDS_Tick()
{
    static unsigned char value = 0x80;
    static unsigned char count = 0;

    // actions
    switch(led_state)
    {
        case SHIFT_RIGHT:
        {
            if(value == 0x01 || (shiftFlag))
            {
                led_state = SHIFT_LEFT; 
                shiftFlag = 0;
            }

            break;
        }
        case SHIFT_LEFT:
        {
            if(value == 0x80 || (shiftFlag))
            {
                led_state = SHIFT_RIGHT; 
                shiftFlag = 0;
            }

            break;
        }
    }

    // transitions
    switch(led_state)
    {
        case SHIFT_RIGHT:
        {
            value >>= 1;
            LED = value;
           
            break;
        }
        case SHIFT_LEFT:
        {
            value <<= 1;
            LED = value;

            break;
        }
    }
}

void ButtonTask() 
{
    button_init();
    for(;;) 
    { 	
        button_tick();
        vTaskDelay(100); 
    } 
}

void LedSecTask() 
{
    LEDS_Init();
    for(;;) 
    { 	
        LEDS_Tick();
        vTaskDelay(200); 
    } 
}

void ButtonPulse(unsigned portBASE_TYPE Priority) 
{
    xTaskCreate(
        ButtonTask, 
        (signed portCHAR *)"ButtonTask", 
        configMINIMAL_STACK_SIZE, 
        NULL, 
        Priority, 
        NULL 
    );
}	

void StartSecPulse(unsigned portBASE_TYPE Priority) 
{
    xTaskCreate(
        LedSecTask, 
        (signed portCHAR *)"LedSecTask", 
        configMINIMAL_STACK_SIZE, 
        NULL, 
        Priority, 
        NULL 
    );
}	
 
int main(void) 
{ 
    DDRA = 0x00; PORTA=0xFF;
    DDRD = 0xFF;

    //Start Tasks  
    StartSecPulse(1);
    ButtonPulse(1);
    
    //RunSchedular 
    vTaskStartScheduler(); 

    return 0; 
}
