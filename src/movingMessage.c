#include "movingMessage.h"

enum mode {
    CLOCK = 1, DATE = 2, DATETIME = 3, TEMPERATURE = 4, MESSAGE = 5, MESSAGE_WITH_TEMP = 6, WIRELESS_DATA = 7
};

unsigned char currentMode = DATE;
unsigned long time = 1262217600UL; //2009 12 31 00 00 00 //2085978496UL;
unsigned short eepromAddress = 0;
unsigned char secondsCounter = 0;
unsigned char changeTimeDigit = 0;
unsigned char changeDateDigit = 0;
unsigned char showDigit = 0;

int main() {
    portsInit();
    //Enable global interupts
    sei();
//    externalInterruptsInit();
    adcInit(); //init Analog->Digital converter
    timer16Init(); //init 16 bit timer counter
    //initializeTimer8_0();  //init 8 bit timer counter 0
    usartInit(F_CPU / (BAUD * 16UL - 1)); //Init USART
    i2cInit((F_CPU - 16 * TWI_SPEED) / (2 * TWI_SPEED)); //Init TWI
    spiMasterInit();
    //initializeTemperatureSensor();
    wirelessTransceiverInit();

    while (1) {
        unsigned char numberOfPackets = 0;
        switch (currentMode) {
            case CLOCK:
                displayTime();
                break;
            case DATE:
                displayDate();
                break;
            case DATETIME:
                displayDatetime();
                break;
            case TEMPERATURE:
                displayTemperature();
                break;
            case MESSAGE:
                clearBoard();
                displayMessage(75, 0);
                break;
            case MESSAGE_WITH_TEMP:
                //displayMessageAndTemperature();
                break;
            case WIRELESS_DATA:
                clearBoard();
                numberOfPackets = doWirelessCommunication();
                if (numberOfPackets > 0) {
                    displayMessage(numberOfPackets * 16, 0);
                }
                //displayStatus();
                break;
            default:
                //select function
                break;
        }

    }// while 1
}

//###################################################
//############ INTERRUPTS ###########################
//###################################################
//ISR(INT0_vect) {
//    currentMode = WIRELESS_DATA;
//}



ISR(USART_RXC_vect)
{
    e2prom_write_byte(eepromAddress, USART_Receive());
    eepromAddress++;
    if (eepromAddress >= 2047) {
        eepromAddress = 0;
        USART_flush();
        Reset_AVR();
    }
    USART_flush();
}

//Timer is execuded every 1/4 second
ISR(TIMER1_COMPA_vect)
{
    if (secondsCounter == 0x04) {
        oneSecondTimer();
        secondsCounter = 0x00;
    }
    unsigned short adcVal = readAdc();
    setMode(adcVal);

    if (showDigit == 0x00)
        showDigit = 0x01;
    else
        showDigit = 0x00;

    secondsCounter++;
}

void oneSecondTimer() {
    packetReceiveTimeout++;
    if (packetReceiveTimeout > 5) {
        packetReceiveTimeout = 0;
    }
    time++;
}

//###################################################
//############ FUNCTIONS ############################
//###################################################

void displayTemperature() {

    unsigned char temperature_data[2] = { 0x00, 0x00 };
    unsigned int fraction = 0;
    unsigned char temperature = 0;
    unsigned char sign = 0;

    //display buffer when reading temperature
    //this is needed to minimize delays beetween board redraw

    //Reset, skip ROM and start temperature conversion
    resetWire(TEMPERATURE_PIN);
    display_buffer(0);
    write_byte(THERM_CMD_SKIPROM, TEMPERATURE_PIN);
    display_buffer(0);
    write_byte(THERM_CMD_CONVERTTEMP, TEMPERATURE_PIN);
    display_buffer(0);
    //Wait until conversion is complete
    //  while(!read_bit());
    //Reset, skip ROM and send command to read Scratchpad
    resetWire(TEMPERATURE_PIN);
    display_buffer(0);
    write_byte(THERM_CMD_SKIPROM, TEMPERATURE_PIN);
    display_buffer(0);
    write_byte(THERM_CMD_RSCRATCHPAD, TEMPERATURE_PIN);
    display_buffer(0);
    //  Read Scratchpad (only 2 first bytes)
    temperature_data[0] = read_byte(TEMPERATURE_PIN);
    display_buffer(0);
    temperature_data[1] = read_byte(TEMPERATURE_PIN);
    display_buffer(0);

    formatTemperature(temperature_data, &sign, &temperature, &fraction);

    unsigned char firstDigit = temperature / 10 + 16;
    unsigned char secondDigit = temperature % 10 + 16;
    unsigned char fractionFirstDigit = fraction / 1000 + 16;
    //unsigned char fractionSecondDigit = (fraction / 100) % 10 + 16;

    unsigned char j = 0;
    for (unsigned char i = 0; i < BOARD_LENGHT; i++) {

        if (i == 30 || i == 35 || i == 40 || i == 45 || i == 50 || i == 55 || i == 60 || i == 65) {
            j = 0;
        }

        if (i < 25) {
            message_buffer[i] = 0x00;
        } else if (i < 30) {
            message_buffer[i] = (sign == 1) ? eeprom_read_byte(&characters[13][j]) : 0x00;
        } else if (i < 35) {
            message_buffer[i] = eeprom_read_byte(&characters[firstDigit][j]);
        } else if (i < 40) {
            message_buffer[i] = eeprom_read_byte(&characters[secondDigit][j]);
        } else if (i < 45) {
            message_buffer[i] = eeprom_read_byte(&characters[12][j]);
        } else if (i < 50) {
            message_buffer[i] = eeprom_read_byte(&characters[fractionFirstDigit][j]);
        } else if (i < 55) {
            message_buffer[i] = 0x00;
        } else if (i < 60) {
            message_buffer[i] = eeprom_read_byte(&characters[35][j]);
        } else if (i < 65) {
            message_buffer[i] = eeprom_read_byte(&characters[94][j]);
        } else if (i < 80) {
            message_buffer[i] = 0x00;
        }
        j++;
    }

}

/****************************************************
 ** Scrolls message
 *****************************************************/
void displayMessage(unsigned int message_lenght, unsigned char start_at) {
    unsigned int char_position = 0;
    unsigned char mode = currentMode;
    unsigned char character_byte = 6;

    while (char_position < message_lenght && mode == currentMode) {
        unsigned char repeat_count = 0;
        //Calculate repeating of same buffer is speed below 10
        if (SPEED < 10) {
            if (repeat_count > 0) {
                repeat_count = repeat_count - 1;
            } else {
                repeat_count = 22 - 2 * SPEED;
            }
        }
        if (repeat_count == 0 && SPEED != 0) {
            unsigned char eeprom_buffer;
            //Prepare new buffer
            if (character_byte == 6) {
                e2prom_read_byte(char_position, &eeprom_buffer);
                char_position++;
                character_byte = 0;
            }

            for (unsigned char k = start_at; k < BOARD_LENGHT - 1; k++) {
                message_buffer[k] = message_buffer[k + 1];
            }
            if (character_byte == 5) {
                message_buffer[BOARD_LENGHT - 1] = 0x00;
            } else {
                message_buffer[BOARD_LENGHT - 1] = eeprom_read_byte(&characters[eeprom_buffer - 32][character_byte]);
            }
            character_byte++;
        }
        display_buffer(SPEED);
    }
}

//******************************************************************
void displayDate() {
    unsigned char x = 0, i = 0;

    formatDateTime(time);

    for (;i < BOARD_LENGHT;) {
        if (date_config[x].position == i && (!date_config[x].blink || (date_config[x].blink && showDigit))) {
            message_buffer[i] = eeprom_read_byte(&characters[date_config[x].digitChar][0]);
            message_buffer[i + 1] = eeprom_read_byte(&characters[date_config[x].digitChar][1]);
            message_buffer[i + 2] = eeprom_read_byte(&characters[date_config[x].digitChar][2]);
            message_buffer[i + 3] = eeprom_read_byte(&characters[date_config[x].digitChar][3]);
            message_buffer[i + 4] = eeprom_read_byte(&characters[date_config[x].digitChar][4]);
            x++;
            i+=5;
        } else if (date_config[x].position == i) {
            x++;
        } else {
            message_buffer[i] = 0x00;
            i++;
        }
    }

    display_buffer(0);
}

//******************************************************************
void displayTime() {
    unsigned char x = 0, i = 0;

    formatDateTime(time);

    for (;i < BOARD_LENGHT;) {
        if (time_config[x].position == i && (!time_config[x].blink || (time_config[x].blink && showDigit))) {
            message_buffer[i] = eeprom_read_byte(&characters[time_config[x].digitChar][0]);
            message_buffer[i + 1] = eeprom_read_byte(&characters[time_config[x].digitChar][1]);
            message_buffer[i + 2] = eeprom_read_byte(&characters[time_config[x].digitChar][2]);
            message_buffer[i + 3] = eeprom_read_byte(&characters[time_config[x].digitChar][3]);
            message_buffer[i + 4] = eeprom_read_byte(&characters[time_config[x].digitChar][4]);
            x++;
            i+=5;
        } else if (time_config[x].position == i) {
            x++;
        } else if (i == 30 || i == 45) {
            for (unsigned char j = 0; j < 5; j++)
                message_buffer[i++] = eeprom_read_byte(&characters[26][j]);
//        } else if (i == 20) {
//            for (unsigned char j = 0; j < 5; j++)
//                message_buffer[i++] = eeprom_read_byte(&characters[65][j]);
//        } else if (i == 25) {
//            for (unsigned char j = 0; j < 5; j++)
//                message_buffer[i++] = eeprom_read_byte(&characters[76][j]);
//        } else if (i == 50) {
//            for (unsigned char j = 0; j < 5; j++)
//                message_buffer[i++] = eeprom_read_byte(&characters[77][j]);
//        } else if (i == 55) {
//            for (unsigned char j = 0; j < 5; j++)
//                message_buffer[i++] = eeprom_read_byte(&characters[73][j]);
//        } else if (i == 60) {
//            for (unsigned char j = 0; j < 5; j++)
//                message_buffer[i++] = eeprom_read_byte(&characters[78][j]);
        } else {
            message_buffer[i] = 0x00;
            i++;
        }
    }
    display_buffer(0);
}

//******************************************************************

void displayDatetime() {
    for (int i = 0; (i < 400) && (currentMode == DATETIME); ++i) {
        displayDate();
    }
    for (int i = 0; (i < 400) && (currentMode == DATETIME); ++i) {
        displayTime();
    }
}
/*
 void displayMessageAndTemperature() {
 unsigned char temp_chart_count = 25;
 unsigned char temperature = 0;
 unsigned char temperature_data[2];

 formatTemperature(temperature_data, 0, &temperature, 0);
 unsigned char firstDigit = temperature / 10;
 unsigned char secondDigit = temperature % 10;
 unsigned char j = 0;
 for (unsigned char i = 0; i < BOARD_LENGHT; i++) {
 if (i == 0 || i == 10 || i == 15 || i == 20 || i == 25) {
 j = 0;
 }
 if (i < 5) {
 message_buffer[i] = eeprom_read_byte(&temperature_chars[firstDigit][j]);
 } else if (i < 10) {
 message_buffer[i] = eeprom_read_byte(&temperature_chars[secondDigit][j]);
 } else if (i < 15) {
 message_buffer[i] = eeprom_read_byte(&temperature_chars[15][j]);
 } else if (i < 20) {
 message_buffer[i] = eeprom_read_byte(&temperature_chars[14][j]);
 } else if (i < 25) {
 message_buffer[i] = eeprom_read_byte(&temperature_chars[11][0]);
 }
 j++;
 }

 displayMessage(EEPROM_SIZE - BOARD_LENGHT, temp_chart_count);

 //display buffer when reading temperature
 //this is needed to minimize delays beetween board redraw

 //Reset, skip ROM and start temperature conversion
 resetWire(TEMPERATURE_PIN);
 display_buffer(SPEED);
 write_byte(THERM_CMD_SKIPROM, TEMPERATURE_PIN);
 display_buffer(SPEED);
 write_byte(THERM_CMD_CONVERTTEMP, TEMPERATURE_PIN);
 display_buffer(SPEED);
 //Wait until conversion is complete
 //	while(!read_bit());
 //Reset, skip ROM and send command to read Scratchpad
 resetWire(TEMPERATURE_PIN);
 display_buffer(SPEED);
 write_byte(THERM_CMD_SKIPROM, TEMPERATURE_PIN);
 display_buffer(SPEED);
 write_byte(THERM_CMD_RSCRATCHPAD, TEMPERATURE_PIN);
 display_buffer(SPEED);
 //	Read Scratchpad (only 2 first bytes)
 temperature_data[0] = read_byte(TEMPERATURE_PIN);
 display_buffer(SPEED);
 temperature_data[1] = read_byte(TEMPERATURE_PIN);
 display_buffer(SPEED);

 }
 */

void setMode(unsigned short value) {
    if (currentMode != CLOCK) {
        changeTimeDigit = 0;
        calculateBlinking(changeTimeDigit, changeDateDigit);
    }
    if (currentMode != DATE) {
        changeDateDigit = 0;
        calculateBlinking(changeTimeDigit, changeDateDigit);
    }

    if ((1024 > value) && (value >= 960)) {
    } else if ((960 > value) && (value >= 896)) {
        //left player = increase;
    } else if ((896 > value) && (value >= 832)) {
        //left player = decrease;
    } else if ((832 > value) && (value >= 768)) {
        //right player = increase;
    } else if ((768 > value) && (value >= 704)) {
        //right player = decrease;
    } else if ((704 > value) && (value >= 640)) {
        changeTime(&time, TRUE, changeTimeDigit, changeDateDigit);
    } else if ((640 > value) && (value >= 576)) {
        changeTime(&time, FALSE, changeTimeDigit, changeDateDigit);
    } else if ((576 > value) && (value >= 512)) {
        currentMode = CLOCK;
        changeTimeDigit++;
        calculateBlinking(changeTimeDigit, changeDateDigit);
        if (changeTimeDigit == 6) {
            changeTimeDigit = 0;
        }
    } else if ((512 > value) && (value >= 448)) {
        currentMode = DATE;
        changeDateDigit++;
        calculateBlinking(changeTimeDigit, changeDateDigit);
        if (changeDateDigit == 4) {
            changeDateDigit = 0;
        }
    } else if ((448 > value) && (value >= 384)) {
        currentMode = CLOCK;
    } else if ((384 > value) && (value >= 320)) {
        currentMode = DATE;
    } else if ((320 > value) && (value >= 256)) {
        currentMode = DATETIME;
    } else if ((256 > value) && (value >= 192)) {
        currentMode = MESSAGE;
    } else if ((192 > value) && (value >= 128)) {
        currentMode = TEMPERATURE;
    } else if ((128 > value) && (value >= 64)) {
        //currentMode = MESSAGE_WITH_TEMP;
    } else if ((64 > value) && (value >= 0)) {
        currentMode = WIRELESS_DATA;
    }
}

void clearBoard() {
    for (unsigned char j = 0; j < BOARD_LENGHT; j++)
        message_buffer[j] = 0x00;
    display_buffer(0);
}

unsigned short readAdc() {
    ADCSR |= (1 << ADSC); //Start conversion
    //Wait for conversion to complete
    while (!(ADCSR & (1 << ADIF)))
        ;
    //Clear ADIF by writing one to it
    ADCSR |= (1 << ADIF);
    return (ADC);
}

