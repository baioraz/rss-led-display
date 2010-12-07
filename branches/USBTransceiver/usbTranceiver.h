#define F_CPU 8000000UL

#define FIFO_STATUS_PIN PIND
#define TXE PD4
#define RXF PD3
#define WR PD5
#define RD PD6

#define READ_TXE() FIFO_STATUS_PIN & (1<<TXE)
#define READ_RXF() FIFO_STATUS_PIN & (1<<RXF)

#define FIFO_RDATA_ENABLE() PORTD&=~(1<<RD)
#define FIFO_RDATA_DISABLE() PORTD|=(1<<RD)

#define FIFO_WDATA_ENABLE() PORTD&=~(1<<WR)
#define FIFO_WDATA_DISABLE() PORTD|=(1<<WR)

#define D0 PD1
#define D1 PC4
#define D2 PC5
#define D3 PC0
#define D4 PD0
#define D5 PC2
#define D6 PC1
#define D7 PC3

#define DATA_D_PIN PIND
#define DATA_C_PIN PINC

#define READ_D0() DATA_D_PIN & (1<<D0)
#define READ_D1() DATA_C_PIN & (1<<D1)
#define READ_D2() DATA_C_PIN & (1<<D2)
#define READ_D3() DATA_C_PIN & (1<<D3)
#define READ_D4() DATA_D_PIN & (1<<D4)
#define READ_D5() DATA_C_PIN & (1<<D5)
#define READ_D6() DATA_C_PIN & (1<<D6)
#define READ_D7() DATA_C_PIN & (1<<D7)

#define D0_EN() PORTD|=(1<<D0)
#define D1_EN() PORTC|=(1<<D1)
#define D2_EN() PORTC|=(1<<D2)
#define D3_EN() PORTC|=(1<<D3)
#define D4_EN() PORTD|=(1<<D4)
#define D5_EN() PORTC|=(1<<D5)
#define D6_EN() PORTC|=(1<<D6)
#define D7_EN() PORTC|=(1<<D7)

#define D0_DIS() PORTD&=~(1<<D0)
#define D1_DIS() PORTC&=~(1<<D1)
#define D2_DIS() PORTC&=~(1<<D2)
#define D3_DIS() PORTC&=~(1<<D3)
#define D4_DIS() PORTD&=~(1<<D4)
#define D5_DIS() PORTC&=~(1<<D5)
#define D6_DIS() PORTC&=~(1<<D6)
#define D7_DIS() PORTC&=~(1<<D7)

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "wireless.h"

void init_ports();
void timer16_init(void);
unsigned char receiveDataFromFifo();
void sendDataToFifo(unsigned char fifoData);
