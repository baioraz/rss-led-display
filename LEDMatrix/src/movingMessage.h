#define F_CPU 16000000UL

#ifndef MOVINGMESSAGE_H_
#define MOVINGMESSAGE_H_

#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "spi.h"
#include "ledboard.h"
#include "e2prom.h"
#include "usart.h"
#include "oneWire.h"
#include "thermometer.h"
#include "datetime.h"
#include "wireless.h"
#include "initializers.h"
#include "font.h"
#include "global.h"

#define SPEED 15

#define Reset_AVR() wdt_enable(WDTO_30MS); while(1) {}
#define INT_PIN PIND
#define READ_INT0() INT_PIN & (1<<PD2)
#define READ_INT1() INT_PIN & (1<<PD3)

void displayMessage(unsigned int message_lenght, unsigned char start_at);
void displayDatetime();
void displayDate();
void displayTime();
void displayTemperature();
void display_scroll_day_and_temp();
void displayMessageAndTemperature();
unsigned short readAdc();
void clearBoard();
void setMode(unsigned short value);
void oneSecondTimer();

void prepareStatusMessage(unsigned char responseBytesCount, unsigned char messageNumber, unsigned char *dataBuffer);

#endif

