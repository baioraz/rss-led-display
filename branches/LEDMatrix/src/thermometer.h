#ifndef THERMOMETER_H_
#define THERMOMETER_H_

#include "oneWire.h"

#define THERM_CMD_CONVERTTEMP 0x44
#define THERM_CMD_RSCRATCHPAD 0xbe
#define THERM_CMD_WSCRATCHPAD 0x4e
#define THERM_CMD_CPYSCRATCHPAD 0x48
#define THERM_CMD_RECEEPROM 0xb8
#define THERM_CMD_RPWRSUPPLY 0xb4
#define THERM_CMD_SEARCHROM 0xf0
#define THERM_CMD_READROM 0x33
#define THERM_CMD_MATCHROM 0x55
#define THERM_CMD_SKIPROM 0xcc
#define THERM_CMD_ALARMSEARCH 0xec
#define THERM_DECIMAL_STEPS_12BIT 625

void formatTemperature(unsigned char *temperature_data, unsigned char *sign, unsigned char *temperature, unsigned int *fraction);
void initializeTemperatureSensor();

#endif


