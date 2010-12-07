#ifndef LEDBOARD_H_
#define LEDBOARD_H_



#include <avr/io.h>
#include <util/delay.h>
#include "global.h"

#define DATA PC3
#define RESET PC0  
#define STORE PC1
#define SHIFT PC2
#define DATA_LOW() PORTC&=~(1<<DATA)
#define DATA_HIGH() PORTC|=(1<<DATA)
#define RESET_LOW() PORTC&=~(1<<RESET)
#define RESET_HIGH() PORTC|=(1<<RESET)
#define STORE_LOW() PORTC&=~(1<<STORE)
#define STORE_HIGH() PORTC|=(1<<STORE)
#define SHIFT_LOW() PORTC&=~(1<<SHIFT)
#define SHIFT_HIGH() PORTC|=(1<<SHIFT)

#define R1 PD5
#define R2 PD6  
#define R4 PD7
#define R1EN() PORTD|=(1<<R1)
#define R2EN() PORTD|=(1<<R2)
#define R4EN() PORTD|=(1<<R4)
#define R1DIS() PORTD&=~(1<<R1)
#define R2DIS() PORTD&=~(1<<R2)
#define R4DIS() PORTD&=~(1<<R4)


#define BOARD_LENGHT 80

//void board_reset( void );
void shift_row(unsigned char row);
void display_buffer(unsigned char speed);



#endif

