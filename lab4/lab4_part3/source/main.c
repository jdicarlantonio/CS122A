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
    static unsigned char shiftFlag = 1;

    // actions
    switch(led_state)
    {
        case SHIFT_RIGHT:
        {
            LED = value;
            value >>= 1;
            if(value == 0x01)
            {
                led_state = SHIFT_LEFT; 
            }

            break;
        }
        case SHIFT_LEFT:
        {
            LED = value;
            value <<= 1;
            if(value == 0x80)
            {
                led_state = SHIFT_RIGHT; 
            }

            break;
        }
    }

    // transitions
    switch(led_state)
    {
        case SHIFT_RIGHT: break;
        case SHIFT_LEFT: break;
    }
}

void LedSecTask() 
{
    LEDS_Init();
    for(;;) 
    { 	
        LEDS_Tick();
        vTaskDelay(250); 
    } 
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
    
    //RunSchedular 
    vTaskStartScheduler(); 

    return 0; 
}
