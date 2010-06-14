#ifndef E2PROM_H_
#define E2PROM_H_

#include <util/twi.h>

#define EEPROM_SIZE 2048UL
#define MAX_TRIES 100
#define TWI_SPEED ((F_CPU < 14318000) ? 400000 : 100000)

int e2prom_read_byte(unsigned short eeaddr, unsigned char *buf);
int e2prom_write_byte(unsigned short eeaddr, char buf);
void i2cInit(unsigned char twbr);

#endif

