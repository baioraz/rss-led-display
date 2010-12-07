#ifndef WIRELESS_H_
#define WIRELESS_H_

#include "spi.h"
#include "ledboard.h"
#include "e2prom.h"
#include "global.h"
#include "font.h"
#include <util/delay.h>
#include <avr/eeprom.h>
#include <math.h>

unsigned char transmitPacket(unsigned char *data);
void setRXmode();
void setTXmode();
//void checkTransceiver();
unsigned char checkReceptionStatus();
void wirelessTransceiverInit();
unsigned char receiveConfigurationPacket(unsigned char *packet);
unsigned char receiveData();
void transmitData(unsigned char *packet);
unsigned char receiveWirelessData();
unsigned char receivePacket(unsigned char firstPacket, unsigned short index);
void prepareStatusMessage(unsigned char responseBytesCount, unsigned char status, unsigned char *dataBuffer);
void displayStatus();

#endif


