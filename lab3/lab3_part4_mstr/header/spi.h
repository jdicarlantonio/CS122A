// Permission to copy is granted provided that this header remains intact. 
// This software is provided with no warranties.

////////////////////////////////////////////////////////////////////////////////
#ifndef SPI_1284_H
#define SPI_1284_H
//this header is used for the atmega 1284 for spi functionality

#include <avr/interrupt.h>

#define DDR_SPI DDRB
#define PORT_SPI PORTB
#define DD_MOSI 5
#define DD_SCK 7
#define DD_SS 4
#define DD_MISO 6

unsigned char receivedData;

//Master code
void SPI_MasterInit(void)
{
    /* Set DDRB to have MOSI, SS, and SCK output, and MIS0 input */
    DDR_SPI = (1<<DD_MOSI)|(1<<DD_SCK)| 0x0F;
    PORT_SPI = ~((1<<DD_MOSI)|(1<<DD_SCK) | 0x0F);
    /* Enable SPI, Master, set clock rate fosc/16 */
    SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
    /*Make sure global interrupts are enabled on the SREG register*/
    SREG |= 0x80;
}

void SPI_MasterTransmit(char cData, char uC)
{
    /* Start transmission */
    if(uC >= 4)
    {
        return;
        
    }
    SPDR = cData;
    //set SS low
//    PORT_SPI |= 0xFE;
    PORT_SPI |= ~(1<<uC);
    /* Wait for transmission complete */
    while(!(SPSR & (1<<SPIF)));
    //set SS high
//    PORT_SPI &= (1<<uC);
    PORT_SPI &= 0xF0;
}

//Servant Code
void SPI_ServantInit(void)
{
    /* Set MISO output, all others input */
    DDR_SPI = (1<<DD_MISO);
    PORT_SPI = ~(1<<DD_MISO);
    /* Enable SPI, SPI_Interrupt */
    SPCR = (1<<SPE)|(1<<SPIE);
    /*Make sure global interrupts are enabled on the SREG register*/
    SREG |= 0x80;
}

ISR(SPI_STC_vect)
{
    /* this is enbaled in withe the SPCR register's "SPI 
     * Interrupt Enable" */
    /* SPDR contains the recieved data, e.g. 
     * unsigned char receivedData = SPDR; */
    receivedData = SPDR;
}
#endif // SPI_1284_H
