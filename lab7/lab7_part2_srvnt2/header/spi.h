#ifndef SPI_H
#define SPI_H

#include <avr/interrupt.h>
#include <avr/io.h>

#define DDR_SPI DDRB
#define DD_MOSI 5
#define DD_MISO 6
#define DD_SCK 7
#define DD_SS 4

unsigned char spdrValue;

void SPI_MasterInit(void)
{
    DDR_SPI = (1 << DD_MOSI) | (1 << DD_SCK) | (1 << DD_SS);
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
    PORTB = ~((1 << DD_MOSI) | (1 << DD_SCK) | (1 << DD_SS));

    SREG |= 0x80;
}

void SPI_MasterTransmit(unsigned char cData)
{
    SPDR = cData;

    PORTB |= (0 << 4);
    while(!(SPSR & (1 << SPIF)));
    PORTB |= (1 << 4);
}

void SPI_ServantInit(void)
{
    DDR_SPI = (1 << DD_MISO);
    PORTB = ~(1 << DD_MISO);
    SPCR = (1 << SPE) | (1 << SPIE);

    SREG |= 0x80;
}

unsigned char flag = 0xFF;

ISR(SPI_STC_vect)
{
//    while(!(SPSR & (1 << SPIF)));
    
//    PORTB |= (0 << 4);
//    return SPDR;
    spdrValue = SPDR;

    if(spdrValue == 0x40)
    {
        flag = 0xFF; 
    }
    if(spdrValue == 0x20)
    {
        flag = 0x00; 
    }
}

#endif
