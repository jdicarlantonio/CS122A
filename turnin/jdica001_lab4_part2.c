/*	Author: Joseph DiCarlantonio
 *  Partner(s) Name: Raymond Chlebeck
 *	Lab Section:
 *	Assignment: Lab 4  Exercise 2
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

enum LEDState 
{
    OFF,
    ON
} led_state, sec_state, half_state, quarter_state;
// LED states for second, half second, and quarter second (1000, 500, and 250 ms)

void LEDQ_Init()
{
    quarter_state = OFF;
}

void LEDQ_Tick()
{
    // actions
    switch(quarter_state)
    {
        case OFF: LED &= ~(1 << 4); break;
        case ON: LED |= 0x10; break;
    }
    
    // transitions
    switch(quarter_state)
    {
        case OFF: quarter_state = ON; break;
        case ON: quarter_state = OFF; break;
    }
}

void LEDH_Init()
{
    half_state = OFF;
}

void LEDH_Tick()
{
    // actions
    switch(half_state)
    {
        case OFF: LED &= ~(1 << 2); break;
        case ON: LED |= 0x04; break;
    }

    // transitions
    switch(half_state)
    {
        case OFF: half_state = ON; break; 
        case ON: half_state = OFF; break; 
    }
}

void LEDS_Init()
{
    sec_state = OFF;
}

void LEDS_Tick()
{
    // actions
    switch(sec_state)
    {
        case OFF: LED &= ~(1 << 0); break;
        case ON: LED |= 0x01; break;
    }

    // transitions
    switch(sec_state)
    {
        case OFF: sec_state = ON; break;
        case ON: sec_state = OFF; break;
    }
}

void LedQuarterTask()
{
    LEDQ_Init();
    for(;;)
    {
        LEDQ_Tick();
        vTaskDelay(2500);
    }
}

void LedHalfTask()
{
    LEDH_Init();
    for(;;)
    {
        LEDH_Tick();
        vTaskDelay(500);
    }
}

void LedSecTask() 
{
    LEDS_Init();
    for(;;) 
    { 	
        LEDS_Tick();
        vTaskDelay(1000); 
    } 
}

void StartQuarterPulse(unsigned portBASE_TYPE Priority) 
{
    xTaskCreate(
        LedQuarterTask, 
        (signed portCHAR *)"LedQuarterTask", 
        configMINIMAL_STACK_SIZE, 
        NULL, 
        Priority, 
        NULL 
    );
}	

void StartHalfPulse(unsigned portBASE_TYPE Priority) 
{
    xTaskCreate(
        LedHalfTask, 
        (signed portCHAR *)"LedHalfTask", 
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
    StartHalfPulse(1);
    StartQuarterPulse(1);

    //RunSchedular 
    vTaskStartScheduler(); 

    return 0; 
}
