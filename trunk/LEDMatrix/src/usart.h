#ifndef USART_H_
#define USART_H_

#include <avr/io.h>
#define BAUD 57600UL

/* Prototypes */
void usartInit( unsigned int baudrate );
unsigned char USART_Receive( void );
void USART_Transmit( unsigned char data );
unsigned char USART_DataReceived( void );
void USART_flush( void );

#endif


