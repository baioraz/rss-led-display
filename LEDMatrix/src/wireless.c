#include "wireless.h"

// transition cyrf6936 into receive mode
void setRXmode() {
    // set up RX.
    SPI_Transmit(0x05, 0x82);
    SPI_Transmit(0x0F, 0x13);
    return;
}

void wirelessTransceiverInit() {
    SPI_Transmit(0x1D, 0x01); // MODE_OVERRIDE, RST (Reset module)

    //SPI_Transmit(0x03,0x2F);  //Power Level +4dBm, data mode 8DR, data code length 64 chip codes. Change to 0F for 32 chip codes
    SPI_Transmit(0x03, 0x0F); //Power Level +4dBm, data mode 8DR, data code length 64 chip codes. Change to 0F for 32 chip codes
    SPI_Transmit(0x06, 0x4A); //
    SPI_Transmit(0x0C, 0xC4); //Set aditional pin as GPIO
    //  SPI_Transmit(0x0D,0x10);    //Set aditional pin as GPIO
    SPI_Transmit(0x0F, 0x27); //Ack enable, Ack 15x, idle mode
    //  SPI_Transmit(0x10,0xEE);    //Use A4 for 32 chips sop code lenght
    SPI_Transmit(0x10, 0xA4); //Use A4 for 32 chips sop code lenght
    SPI_Transmit(0x11, 0x05); //DATA32_THOLD
    SPI_Transmit(0x12, 0x0E); //DATA64_THOLD
    SPI_Transmit(0x15, 0x14); //CRC LSB
    SPI_Transmit(0x16, 0x14); //CRC MSB
    SPI_Transmit(0x1B, 0x55); //TX_OFFSET_LSB
    SPI_Transmit(0x1C, 0x05); //TX_OFFSET_MSB
    //    SPI_Transmit(0x0D, 0x40); // IRQ polarity HIGH, default LOW

    SPI_Transmit(0x26, 0x08); // XTAL_CFG  (must)
    //SPI_Transmit(0x28,0x02);        // (must)
    SPI_Transmit(0x32, 0x3C); // AUTO_CAL_TIME  (must)
    SPI_Transmit(0x35, 0x14); // AUTO_CAL_OFFSET  (must)
}

unsigned char receivePacket(unsigned char firstPacket, unsigned short index) {
    unsigned char newdata = 0;

    while (!newdata && (packetReceiveTimeout < 5)) {
        newdata = checkReceptionStatus();
        if (firstPacket == 1) {
            displayStatus();
        }
        display_buffer(0);
    }
    if (packetReceiveTimeout == 5) {
        return 0;
    }

    if (newdata) {
        rssi = SPI_Receive(0x13) & 0x1F; //RSSI and only last 5 bits interesting
        packetReceiveTimeout = 0;
        SPI_Transmit(0x07, 0x80);
        for (unsigned char i = 0; i < 16; i++) {
            unsigned char receivedByte = SPI_Receive(0x21);
            if (firstPacket) {
                configuration[i] = receivedByte;
            } else {
                e2prom_write_byte(index + i, receivedByte);
            }
//else {
//                eeprom_write_byte((uint8_t*) ind, receivedByte);
//                ind++;
//            }
            display_buffer(0);
        }
    }

    return newdata;
}

void displayStatus() {
    unsigned char percent = (100 * rssi) / 31;
    unsigned char firstDigit = percent / 100;
    unsigned char secondDigit = percent % 100 / 10;
    unsigned char thirdDigit = percent % 10;

    char message[6] = { 'S', 'i', 'g', 'n', 'a', 'l' };
    unsigned char x = 15;
    for (unsigned char i = 0; i < 6; i++) {
        for (unsigned char j = 0; j < 5; j++)
            message_buffer[x++] = eeprom_read_byte(&characters[message[i] - 32][j]);
    }
    x = 50;
    for (unsigned char j = 0; j < 5; j++)
        message_buffer[x++] = (firstDigit > 0) ? eeprom_read_byte(&characters[firstDigit + 16][j]) : 0x00;
    for (unsigned char j = 0; j < 5; j++)
        message_buffer[x++] = (secondDigit > 0 || firstDigit > 0) ? eeprom_read_byte(&characters[secondDigit + 16][j]) : 0x00;
    for (unsigned char j = 0; j < 5; j++)
        message_buffer[x++] = eeprom_read_byte(&characters[thirdDigit + 16][j]);
    for (unsigned char j = 0; j < 5; j++)
        message_buffer[x++] = eeprom_read_byte(&characters[5][j]); //percent symbol
    display_buffer(0);
}

unsigned char receiveData() {
    unsigned short index = 0;
    char message[12] = { 'r', 'e', 'c', 'e', 'i', 'v', 'i', 'n', 'g', '.', '.', '.' };
    unsigned char x = 0;
    for (unsigned char i = 0; i < 12; i++) {
        for (unsigned char j = 0; j < 5; j++)
            message_buffer[x++] = eeprom_read_byte(&characters[message[i] - 32][j]);
    }
    for (unsigned char i = 0; i < configuration[0]; i++) {
        //message_buffer[41] = numberOfPackets << 1;
        //message_buffer[40] = i << 1;

        setRXmode();
        unsigned char status = receivePacket(0, index);
        if (!status) {
            break;
        }
        index = index + 16;

        unsigned char percent = (100 * (i + 1)) / configuration[0];
        unsigned char firstDigit = percent / 100;
        unsigned char secondDigit = percent % 100 / 10;
        unsigned char thirdDigit = percent % 10;

        x = 60;
        for (unsigned char j = 0; j < 5; j++)
            message_buffer[x++] = (firstDigit > 0) ? eeprom_read_byte(&characters[firstDigit + 16][j]) : 0x00;
        for (unsigned char j = 0; j < 5; j++)
            message_buffer[x++] = (secondDigit > 0 || firstDigit > 0) ? eeprom_read_byte(&characters[secondDigit + 16][j]) : 0x00;
        for (unsigned char j = 0; j < 5; j++)
            message_buffer[x++] = eeprom_read_byte(&characters[thirdDigit + 16][j]);
        for (unsigned char j = 0; j < 5; j++)
            message_buffer[x++] = eeprom_read_byte(&characters[5][j]); //percent symbol

        display_buffer(0);
    }

    if (index == (configuration[0] * 16)) {
        return TRUE;
    }
    return FALSE;
}

unsigned char receiveWirelessData() {
    unsigned char dataReceptionStatus = 0;

    //Receive configuration packet
    receivePacket(TRUE, 0);

    //Receive data
    dataReceptionStatus = receiveData();

    if (!dataReceptionStatus) {
        char error_message[16] = { 'R', 'e', 'c', 'e', 'p', 't', 'i', 'o', 'n', ' ', 'e', 'r', 'r', 'o', 'r', ' ' };
        unsigned char x = 0;
        for (unsigned char i = 0; i < 16; i++) {
            for (unsigned char j = 0; j < 5; j++)
                message_buffer[x++] = eeprom_read_byte(&characters[error_message[i] - 32][j]);
        }
        for (unsigned char i = 0; i < 200; i++) {
            display_buffer(0);
        }
        configuration[0] = 0;
    }

    return configuration[0];
}

unsigned char checkReceptionStatus() {

    unsigned char status = 0;
    unsigned char timeout = 0;

    if (!(SPI_Receive(0x05) & (1 << 7))) {

        status = status | SPI_Receive(0x07);
        //        message_buffer[10] = status;
        //        message_buffer[11] = status<<1;
        //        display_buffer(message_buffer, 0);
        if (status & 0x01) {
            SPI_Transmit(0x05, 0x82); //RX_GO
            status = 0x00;
            timeout = 0;// transmission failed.
        }
        if (status & (1 << 2)) {
            SPI_Transmit(0x05, 0x82); //RX_GO
            status = 0x00;
            timeout = 0;// buffer fail
        } else if (status & 0x02) {
            SPI_Transmit(0x05, 0x82); //RX_GO
            status = 0x00;
            timeout = 0;
            unsigned char rxStatus = SPI_Receive(0x08);
            //            message_buffer[14] = rxStatus;
            //            message_buffer[15] = rxStatus<<1;
            //            display_buffer(message_buffer, 0);
            //if error set required bits and continue
            if ((rxStatus & 0x40) || (rxStatus & 0x20) || (rxStatus & 0x10) || (rxStatus & 0x08) || !(!(rxStatus & 0x02) & (rxStatus & 0x01))) {
                SPI_Transmit(0x05, 0x82); //RX_GO
                SPI_Transmit(0x07, 0x80); //Set Receive Overwrite Interrupt Status
            } else {
                if (!(rxStatus & 0x08)) {
                    return 1; // transmission sucseded.
                }
            }
        } else if (timeout >= 10) {
            SPI_Transmit(0x05, 0x82); //RX_GO
            status = 0x00;
            timeout = 0; // transmission timeout
        } else if (!(status & (1 << 6))) { // <don't need 1<<6
            timeout++;// transmission timeout
        } else {
            //timeout++;// transmission timeout
        }
    }

    return 0;
}

