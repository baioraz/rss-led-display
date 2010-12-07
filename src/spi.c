#include "spi.h"

#define SS_PORT PB2
#define MOSI_PORT PB3
#define MISO_PORT PB4
#define SCK_PORT PB5
#define IRQ_PORT PD2
#define IRQ_PIN PINB2
#define PACTL_PIN PINB1

#define DESELECT_SLAVE() PORTB|=(1<<SS_PORT)
#define SELECT_SLAVE() PORTB&=~(1<<SS_PORT)
#define MISO_LOW() PORTB&=~(1<<MISO_PORT)


/****************************************************
 ** initialize SPI master
 *****************************************************/
void spiMasterInit(void) {
    /* Enable SPI, Master, set clock rate fck/4 */
    SPCR = (1 << SPE) | (1 << MSTR);
    //  SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0); //f/16
}

/****************************************************
 ** SPI writing
 *****************************************************/
void SPI_Transmit(unsigned char addr, unsigned char value) {
    SELECT_SLAVE();
    SPDR = addr | 0x80;
    /* Wait for transmission complete */
    while (!(SPSR & (1 << SPIF)))
        ;
    SPDR = value;
    /* Wait for transmission complete */
    while (!(SPSR & (1 << SPIF)))
        ;
    DESELECT_SLAVE();
}

/****************************************************
 ** SPI reading
 *****************************************************/
unsigned char SPI_Receive(unsigned char addr) {
    SELECT_SLAVE();
    SPDR = addr;
    /* Wait for transmission complete */
    while (!(SPSR & (1 << SPIF)))
        ;
    /* Write 8 zero's in case to read data from slave */
    SPDR = 0x00;
    /* Wait for reception complete */
    while (!(SPSR & (1 << SPIF)))
        ;
    //shift dummy data again, because first shift for some unknown reason gives 0xFF;
    //  SPDR = 0x00;
    /* Wait for reception complete */
    //  while(!(SPSR & (1<<SPIF)));
    DESELECT_SLAVE();
    /* Return Data Register */
    return SPDR;
}
