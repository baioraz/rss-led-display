#ifndef ONEWIRE_H_
#define ONEWIRE_H_


#include <avr/io.h>
#include <util/delay.h>

#define TEMPERATURE_PIN 1
#define THERM_PORT PORTD
#define THERM_DDR DDRD
#define THERM_PIN PIND
#define THERM_DQ PD4


/* Utils */
#define ONEWIRE_INPUT_MODE(pin) if (pin == TEMPERATURE_PIN) {THERM_DDR&=~(1<<THERM_DQ);}// else {THERM_DDR&=~(1<<THERM_DQ);}
#define ONEWIRE_OUTPUT_MODE(pin) if (pin == TEMPERATURE_PIN) THERM_DDR|=(1<<THERM_DQ);
#define ONEWIRE_LOW(pin) if (pin == TEMPERATURE_PIN) {THERM_PORT&=~(1<<THERM_DQ);}// else {THERM_PORT&=~(1<<THERM_DQ);}
#define ONEWIRE_HIGH(pin) if (pin == TEMPERATURE_PIN) THERM_PORT|=(1<<THERM_DQ);

#define ONEWIRE_READ_PIN() THERM_PIN & (1<<THERM_DQ)

unsigned char resetWire(unsigned char pin);
unsigned char read_bit(unsigned char pin);
unsigned char read_byte(unsigned char pin);
void write_bit(unsigned char bit, unsigned char pin);
void write_byte(unsigned char byte, unsigned char pin);


#endif


