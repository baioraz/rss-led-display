#include "thermometer.h"

/*
void read_temperature()
{
	//Reset, skip ROM and start temperature conversion
	resetWire();
	write_byte(THERM_CMD_SKIPROM);
	write_byte(THERM_CMD_CONVERTTEMP);
	//Wait until conversion is complete
	while(!read_bit());
	//Reset, skip ROM and send command to read Scratchpad
	resetWire();
	write_byte(THERM_CMD_SKIPROM);
	write_byte(THERM_CMD_RSCRATCHPAD);

	//Read Scratchpad (only 2 first bytes)
	temperature_data[0]=read_byte();
	temperature_data[1]=read_byte();
	//resetWire();
}
*/

void initializeTemperatureSensor() {
    resetWire(1);
    write_byte(THERM_CMD_SKIPROM, 1);
    write_byte(THERM_CMD_WSCRATCHPAD, 1);
    write_byte(0x80, 1);
    write_byte(0x00, 1);
    write_byte(0x7F, 1);
    resetWire(1);
    write_byte(THERM_CMD_SKIPROM, 1);
    write_byte(THERM_CMD_CPYSCRATCHPAD, 1);
}

void formatTemperature(unsigned char *temperature_data, unsigned char *sign, unsigned char *temperature, unsigned int *fraction) {
	unsigned char temperatureMSB = 0;
	unsigned char temperatureLSB = 0;
	unsigned char shiftedMsb = 0;
	unsigned char shiftedLsb = 0;

	//MSB format:
	// bit15	bit14	bit13	bit12	bit11	bit10	bit9	bit8
	// S		S		S		S		S		2^6		2^5		2^4
	// if negative temp all S=1, otherwise S=0

  	temperatureMSB = temperature_data[1]; 

	//LSB format:
	// bit7		bit6 	bit5 	bit4 	bit3 	bit2 	bit1 	bit0
	// 2^3  	2^2  	2^1  	2^0  	2^-1 	2^-2 	2^-3 	2^-4

  	temperatureLSB = temperature_data[0]; 


	//Check if sign is present.
  	if(temperatureMSB>=8)
    	*sign = 1;

	shiftedLsb = temperatureLSB >> 4;
	shiftedMsb = temperatureMSB << 4;
				
	*temperature = shiftedMsb | shiftedLsb;

	//leave only fraction in lsb and convert to decimal number from 0626 to 9375
	*fraction = (temperatureLSB&0x0F) * THERM_DECIMAL_STEPS_12BIT; 

}


