// EEPROM functions
// NOTE: These are the functions only for a 24C08!
// If you use a smaller/bigger E2PROM, the I2C communication works different!
// Check the datasheet of the EEPROM you'd like to use and modify the
// functions. i.e. smaller I2C EEPROM's already have the high-byte address
// information in the inital byte (which is 0xa0 or 0xa1 in this case).

#include "e2prom.h"


void i2cInit(unsigned char twbr) {
  /* initialize TWI clock: TWPS = 0 => prescaler = 1 */
#if defined(TWPS0)
  /* has prescaler (mega128 & newer) */
  TWSR = 0;
#endif
  TWBR = twbr;
  
}

int e2prom_read_byte(unsigned short eeaddr, unsigned char *buf)
{
  unsigned char n = 0;
  	// send 0xA0
	// 0xA0 = 1010 000 0
	// 4 bits:   <a..device-indentifier>
	// 3 bits:   <device-address set with chip pins>
	// last bit: <0..write>
	unsigned char sla = 0xA0 | (((eeaddr >> 8) & 0x07) << 1);
	unsigned char block_address = eeaddr;

restart:
  if (n++ >= MAX_TRIES)
    return -1;

begin:
	// send start cond.
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	//Wait for TWINT Flag set. This indicates that the START condition has been transmitted
	while (!(TWCR & (1 << TWINT)));

	if (TW_STATUS == TW_MT_ARB_LOST) goto begin;
	if ( (TW_STATUS != TW_REP_START) && (TW_STATUS != TW_START)) {
		return -1;
	}
	
	// send 0xa0
	// 0xa0 = 1010 000 0
	// 4 bits:   <a..device-indentifier>
	// 3 bits:   <device-address set with chip pins>
	// last bit: <0..write>
	TWDR = sla | TW_WRITE;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	if (TW_STATUS == TW_MT_SLA_NACK) goto restart;
	if (TW_STATUS == TW_MT_ARB_LOST) goto begin;
	if (TW_STATUS != TW_MT_SLA_ACK) goto error;
	
	// send low 8 bits of eeaddr 
	TWDR = block_address;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	if (TW_STATUS == TW_MT_DATA_NACK) goto restart;
	if (TW_STATUS == TW_MT_ARB_LOST) goto begin;
	if (TW_STATUS != TW_MT_DATA_ACK) goto error;
	
	// send high 8 bits of eeaddr
//	TWDR = eeaddr << 8;
//	TWCR = (1 << TWINT) | (1 << TWEN);
//	while (!(TWCR & (1 << TWINT)));
//	if (TW_STATUS == TW_MT_DATA_NACK) goto restart;
//	if (TW_STATUS == TW_MT_ARB_LOST) goto begin;
//	if (TW_STATUS != TW_MT_DATA_ACK) goto error;
	
	// send start cond.
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	if (TW_STATUS == TW_MT_ARB_LOST) goto begin;
	if ( (TW_STATUS != TW_REP_START) && (TW_STATUS != TW_START)) {
		return -1;
	}

	// send 0xa1
	// 0xa0 = 1010 000 1
	// 4 bits:   <a..device-indentifier>
	// 3 bits:   <device-address set with chip pins>
	// last bit: <1..read>
	TWDR = sla | TW_READ;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	if (TW_STATUS == TW_MR_SLA_NACK) goto quit;
	if (TW_STATUS == TW_MR_ARB_LOST) goto begin;
	if (TW_STATUS != TW_MR_SLA_ACK) goto error;
	
	// start read transmission
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));

	switch ((TW_STATUS)) {
		case TW_MR_DATA_NACK:
			// FALLTHROUGH 
		case TW_MR_DATA_ACK:
			*buf = TWDR;
			break;
		default:
			goto error;
	}
	
quit:
	//stop condition
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
	return 1;

error:
	//stop condition
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
	return -1;
}



int e2prom_write_byte(unsigned short eeaddr, char buf) {

	unsigned char n = 0;
	// send 0xA0
	// 0xA0 = 1010 000 0
	// 4 bits:   <a..device-indentifier>
	// 3 bits:   <device-address set with chip pins>
	// last bit: <0..write>
	unsigned char sla = 0xA0 | (((eeaddr >> 8) & 0x07) << 1);
	unsigned char block_address = eeaddr;

restart:
  if (n++ >= MAX_TRIES)
    return -1;

begin:

	// start cond.
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	if (TW_STATUS == TW_MT_ARB_LOST) goto begin;
	if ( (TW_STATUS != TW_REP_START) && (TW_STATUS != TW_START)) {
		return -1;
	}

  	TWDR = sla | TW_WRITE;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	if (TW_STATUS == TW_MT_SLA_NACK) goto restart;
	if (TW_STATUS == TW_MT_ARB_LOST) goto begin;
	if (TW_STATUS != TW_MT_SLA_ACK) goto error;

	// send low 8 bits of eeaddr 
	TWDR = block_address;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	if (TW_STATUS == TW_MT_DATA_NACK) goto quit;
	if (TW_STATUS == TW_MT_ARB_LOST) goto begin;
	if (TW_STATUS != TW_MT_DATA_ACK) goto error;

	//if using larger eeprom than 24c08 we must send another address byte
//	TWDR = block_address;
//	TWCR = (1 << TWINT) | (1 << TWEN);
//	while (!(TWCR & (1 << TWINT)));
//	if (TW_STATUS == TW_MT_DATA_NACK) goto quit;
//	if (TW_STATUS == TW_MT_ARB_LOST) goto begin;
//	if (TW_STATUS != TW_MT_DATA_ACK) goto error;

	// put byte into data register and start transmission
	TWDR = buf;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	if (TW_STATUS != TW_MT_DATA_ACK) goto error;



quit:
	// send stop condition
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
	return 1;

error:
	// send stop condition
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
	return -1;
}
