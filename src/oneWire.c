#include "oneWire.h"

//////////////////////////////////////////////////////////////////////////////
// resetWire - performs a reset on the one-wire bus and
// returns the presence detect. Reset is 480us. Presence checked
// another 60us later. And waiw until it is found
//
unsigned char resetWire(unsigned char pin)
{
	unsigned char i;

	//Pull line low and wait for 480uS
	ONEWIRE_LOW(pin);
	ONEWIRE_OUTPUT_MODE(pin);

	_delay_us(400);
	//Release line and wait for 60uS
	ONEWIRE_INPUT_MODE(pin);
	_delay_us(50);
	//Store line value and wait until the completion of 480uS period
	i = ONEWIRE_READ_PIN();  //(THERM_PIN & (1<<THERM_DQ));
	_delay_us(100);
	//Return the value
  	return(i);               //presence signal returned
}     

//////////////////////////////////////////////////////////////////////////////
// read_bit - reads a bit from the one-wire bus. The delay
// required for a read is 15us, so the DELAY routine won't work.
// We put our own delay function in this routine in the form of a
// for() loop.
//
unsigned char read_bit(unsigned char pin)
{
   	unsigned char bit=0;
	//Pull line low for 1uS
	ONEWIRE_LOW(pin);
	ONEWIRE_OUTPUT_MODE(pin);
	_delay_us(1);
	//Release line and wait for 14uS
	ONEWIRE_INPUT_MODE(pin);
	_delay_us(14);
	//Read line value
	if(ONEWIRE_READ_PIN()) bit=1;
	//Wait for 45uS to end and return read value
	_delay_us(40);
	return bit;
}

//-----------------------------------------------------------------------------
// Read 1-Wire data byte and return it
//
unsigned char read_byte(unsigned char pin)
{
	unsigned char i=8, n=0;
	while(i--){

	//Shift one position right and store read value
	n>>=1;
	n|=(read_bit(pin)<<7);
	}
	return n;
}

//-----------------------------------------------------------------------------
// Send a 1-Wire write bit. Provide 10us recovery time.
//
void write_bit(unsigned char bit, unsigned char pin)
{
  	//Pull line low for 1uS
	ONEWIRE_LOW(pin);
	ONEWIRE_OUTPUT_MODE(pin);
	_delay_us(1);
	//If we want to write 1, release the line (if not will keep low)
	if(bit) {ONEWIRE_INPUT_MODE(pin);}
	//Wait for 60uS and release the line
	_delay_us(55);
	ONEWIRE_INPUT_MODE(pin);
}

//-----------------------------------------------------------------------------
// Write 1-Wire data byte
//
void write_byte(unsigned char byte, unsigned char pin)
{ 
	unsigned char i=8;
	while(i--){
	//Write actual bit and shift one position right to makethe next bit ready
	write_bit(byte&1, pin);
	byte>>=1;
	}
}
