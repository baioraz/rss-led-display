#ifndef SPI_H_
#define SPI_H_

#include <avr/io.h>

void spiMasterInit(void);
void SPI_Transmit(unsigned char addr, unsigned char value);
unsigned char SPI_Receive(unsigned char addr);

#endif


