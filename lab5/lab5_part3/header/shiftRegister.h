#ifndef SHIFTREGISTER_H
#define SHIFTREGISTER_H

#include <avr/io.h>
#include <util/delay.h>

#define PORT PORTB  
#define DDR DDRB
#define DATA PB0            // data pin
#define SHIFT_CLK PB1       // shift clock
#define STORE_CLK PB2       // store block
#define SHIFT_CLR PB3

// second shift register
#define DATAD PD0            // data pin
#define SHIFT_CLKD PB1       // shift clock
#define STORE_CLKD PB2       // store block
#define SHIFT_CLRD PB3

void shiftInit()
{
    // init DDR
    DDR |= ((1 << SHIFT_CLK) | (1 << STORE_CLK) | (1 << DATA) | (1 << SHIFT_CLR));
    PORT |= (1 << SHIFT_CLR);

    DDRD |= ((1 << SHIFT_CLKD) | (1 << STORE_CLKD) | (1 << DATAD) | (1 << SHIFT_CLRD));
    PORTD |= (1 << SHIFT_CLRD);
}

// data shift high
void shiftHigh(unsigned char reg)
{
    switch(reg)
    {
        case 1: PORT |= (1 << DATA); break;
        case 2: PORTD |= (1 << DATAD); break;
    }
}

// data shift low
void shiftLow(unsigned char reg)
{
    switch(reg)
    {
        case 1: PORT &= (~(1 << DATA)); break;
        case 2: PORTD &= (~(1 << DATAD)); break;
    }
}

// pulse the shift clock
void shiftPulse(unsigned char reg)
{
    switch(reg)
    {
        case 1:
        {
            PORT |= (1 << SHIFT_CLK);
            PORT &= (~(1 << SHIFT_CLK));

            break;
        }
        case 2:
        {
            PORTD |= (1 << SHIFT_CLKD);
            PORTD &= (~(1 << SHIFT_CLKD));

            break;
        }
    }
}

// pulse the store clock
void shiftLatch(unsigned char reg)
{
    switch(reg)
    {
        case 1:
        {
            PORT |= (1 << STORE_CLK);
            _delay_loop_1(1); // delay for data storage
            PORT &= (~(1 << STORE_CLK));
            _delay_loop_1(1);

            break;
        }
        case 2:
        {
            PORTD |= (1 << STORE_CLKD);
            _delay_loop_1(1); // delay for data storage
            PORTD &= (~(1 << STORE_CLKD));
            _delay_loop_1(1);

            break;
        }
    }
}

void shiftWrite(uint8_t data, unsigned char reg)
{
    // send all 8 bits one at a time
    for(uint8_t i = 0; i < 8; i++)
    {
        // output data according to MSB
        if(data & 0b10000000)
        {
            // MSB is one
            shiftHigh(reg);
        }
        else
        {              
            // MSB is zero
            shiftLow(reg);
        }

        shiftPulse(reg);  // pulse shift clock
        data = data << 1;  // shift MSB
    }

    // send bits to output latch
    shiftLatch(reg);
}

void Wait()
{
    for(uint8_t i=0;i<50;i++)
    {
        _delay_loop_2(0);
    }
}

void shiftClear(unsigned char reg)
{
    switch(reg)
    {
        case 1:
        {
            PORT &= (~(1 << SHIFT_CLR));
            shiftPulse(reg);
            PORT |= (1 << SHIFT_CLR);
            shiftPulse(reg);
        }
        case 2:
        {
            PORTD &= (~(1 << SHIFT_CLRD));
            shiftPulse(reg);
            PORTD |= (1 << SHIFT_CLRD);
            shiftPulse(reg);
        }
    }
}

#endif
