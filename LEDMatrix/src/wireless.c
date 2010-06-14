#include "wireless.h"

#define WTXC			0x02
#define WTXE			0x01
#define WRXC			0x02
#define WRXE			0x01
#define badCRC		0x08

enum wireless_operation {
    READ = 1, WRITE = 2, GETDATA = 3
};

// transition cyrf6936 into receive mode
void setRXmode() {
    // set up RX.

    SPI_Transmit(0x05, 0x80);
    SPI_Transmit(0x0F, 0x13);

    return;
}

// transition cyrf6936 into transmit mode
void setTXmode() {
    SPI_Transmit(0x0F, 0x27);
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

    SPI_Transmit(0x26, 0x08); // XTAL_CFG  (must)
    //SPI_Transmit(0x28,0x02);        // (must)
    SPI_Transmit(0x32, 0x3C); // AUTO_CAL_TIME  (must)
    SPI_Transmit(0x35, 0x14); // AUTO_CAL_OFFSET  (must)
}

unsigned char receivePacket(unsigned char *packet, unsigned char firstPacket, unsigned char configurationData, unsigned short index) {

    unsigned char newdata=0;
    //message_buffer[19] = packetReceiveTimeout << 1;
    //display_buffer(0);
    while (!newdata && (packetReceiveTimeout < 5)) {
        newdata = checkReceptionStatus(message_buffer);
        if (firstPacket == 1) {
            displayStatus();
        }
        display_buffer(0);
    }
    if (packetReceiveTimeout == 5) {
        return 0;
    }
    //display_buffer(0);

    if (newdata) {
        rssi = SPI_Receive(0x13) & 0x1F; //RSSI and only last 5 bits interesting
        packetReceiveTimeout = 0;
        SPI_Transmit(0x07,0x80);
        unsigned short ind = 256 + index;
        for (unsigned char i=0; i<16; i++){
            unsigned char receivedByte = SPI_Receive(0x21);
            if (firstPacket) {
                packet[i] = receivedByte;
            } else if (!configurationData) {
                //_delay_us(10);
                e2prom_write_byte(index + i, receivedByte);
            } else {
                eeprom_write_byte((uint8_t*)ind, receivedByte);
                ind++;
            }
            display_buffer(0);
        }
    }

    return newdata;
}

void displayStatus(){
    unsigned char percent = (100 * rssi) / 31;
    unsigned char firstDigit = percent / 100;
    unsigned char secondDigit = percent % 100 / 10;
    unsigned char thirdDigit = percent % 10;

    char message[6] = {'S','i','g','n','a','l'};
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

//void displayChannel(){
//    unsigned char channel = SPI_Receive(0x00);
//    unsigned char channelTenths = channel / 10;
//    unsigned char channelOnes = channel % 10;
//    unsigned char percent = (100 * rssi) / 31;
//    unsigned char firstDigit = percent / 100;
//    unsigned char secondDigit = percent % 100 / 10;
//    unsigned char thirdDigit = percent % 10;
//
//    char message[16] = {'2','4',' ',' ','M','H','z',' ','r','s','s','i',' ',' ',' ','%'};
//    unsigned char x = 0;
//    for (unsigned char i = 0; i < 16; i++) {
//        for (unsigned char j = 0; j < 5; j++)
//            message_buffer[x++] = eeprom_read_byte(&characters[message[i] - 32][j]);
//    }
//    x = 10;
//    for (unsigned char j = 0; j < 5; j++)
//        message_buffer[x++] = (channelTenths > 0) ? eeprom_read_byte(&characters[channelTenths + 16][j]) : 0x00;
//    for (unsigned char j = 0; j < 5; j++)
//        message_buffer[x++] = (channelOnes > 0) ? eeprom_read_byte(&characters[channelOnes + 16][j]) : 0x00;
//    x = 60;
//    for (unsigned char j = 0; j < 5; j++)
//        message_buffer[x++] = (firstDigit > 0) ? eeprom_read_byte(&characters[firstDigit + 16][j]) : 0x00;
//    for (unsigned char j = 0; j < 5; j++)
//        message_buffer[x++] = (secondDigit > 0 || firstDigit > 0) ? eeprom_read_byte(&characters[secondDigit + 16][j]) : 0x00;
//    for (unsigned char j = 0; j < 5; j++)
//        message_buffer[x++] = eeprom_read_byte(&characters[thirdDigit + 16][j]);
//    display_buffer(0);
//}

unsigned char receiveConfigurationPacket(unsigned char *packet) {
    unsigned char packetReceived = 0;
    setRXmode();
    packetReceived = receivePacket(packet, 1, 0, 0);
    return packetReceived;
}

unsigned char receiveData(unsigned char numberOfPackets, unsigned char *packet, unsigned char isConfig) {
    unsigned short index = 0;

    char message[12] = {'r','e','c','e','i','v','i','n','g','.','.','.'};
    unsigned char x = 0;
    for (unsigned char i = 0; i < 12; i++) {
        for (unsigned char j = 0; j < 5; j++)
            message_buffer[x++] = eeprom_read_byte(&characters[message[i] - 32][j]);
    }
    for (unsigned char i = 0; i < numberOfPackets; i++) {
        //message_buffer[41] = numberOfPackets << 1;
        //message_buffer[40] = i << 1;

        setRXmode();
        unsigned char status = receivePacket(packet, 0, isConfig, index);
        if (!status) {
            break;
        }
        index = index + 16;

        unsigned char percent = (100 * (i + 1)) / numberOfPackets;
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

    if (index == (numberOfPackets * 16)) {
        return TRUE;
    }
    return FALSE;
}

void transmitData(unsigned char *packet) {
    unsigned short index = 256;
    unsigned char h = 1;
    for (unsigned char i = 0; i < 16; i++) {
            for (int j = 0; j < 16; ++j) {
                packet[j] = eeprom_read_byte((uint8_t*)(index+j));
            }

            for (int var = 0; var < 16; ++var) {
                message_buffer[var+30] = packet[var];
            }
            index = index+16;

            _delay_ms(20);
            setTXmode();
            transmitPacket(packet);

            message_buffer[1] = h << 1;
            for (unsigned char x = 0;x<20;x++){
                display_buffer(0);
            }
            h++;
    }
}

unsigned char doWirelessCommunication() {
    unsigned char operation = 255;
    unsigned char numberOfPackets = 0;
    unsigned char packet[16];

    setRXmode();
    setTXmode();
    //message_buffer[0] = 0x02;
    //display_buffer(0);
    if (1 == receiveConfigurationPacket(packet)) {
        operation = packet[1];
        numberOfPackets = packet[0];
    }
//    message_buffer[22] = numberOfPackets<<1;
//    display_buffer(0);
    int operationStatus = FALSE;
    if (operation == WRITE) {
        operationStatus = receiveData(numberOfPackets, packet, 1);
        prepareStatusMessage(0, operationStatus, packet);
        //_delay_ms(20);
        setTXmode();
        transmitPacket(packet);
    }
    if (operation == READ) {
        prepareStatusMessage(16, TRUE, packet);
        //_delay_ms(20);
        setTXmode();
        transmitPacket(packet);
        transmitData(packet);
    }
    if (operation == GETDATA) {
        operationStatus = receiveData(numberOfPackets, packet, 0);
        prepareStatusMessage(0, operationStatus, packet);
        if (!operationStatus) {
            char error_message[16] = {'R','e','c','e','p','t','i','o','n',' ','e','r','r','o','r',' '};
            unsigned char x = 0;
            for (unsigned char i = 0; i < 16; i++) {
                for (unsigned char j = 0; j < 5; j++)
                    message_buffer[x++] = eeprom_read_byte(&characters[error_message[i] - 32][j]);
            }
            for (unsigned char i = 0; i < 200; i++) {
                display_buffer(0);
            }
            numberOfPackets = 0;
        }
        //_delay_ms(20);
        setTXmode();
        transmitPacket(packet);
    }
    return numberOfPackets;

}

void prepareStatusMessage(unsigned char responseBytesCount, unsigned char status, unsigned char *dataBuffer) {
    dataBuffer[0] = responseBytesCount;
    if (status == 1) {
        dataBuffer[1] = 'O';
        dataBuffer[2] = 'K';
        for (unsigned char i = 3; i < 16; i++) {
            dataBuffer[i] = 0;
        }
    } else {
        dataBuffer[1] = 'X';
        dataBuffer[2] = 'X';
        for (unsigned char i = 3; i < 16; i++) {
            dataBuffer[i] = 0;
        }
    }
}

unsigned char checkReceptionStatus() {

    unsigned char status = 0;
    unsigned char timeout = 0;

    if (!(SPI_Receive(0x05) & (1<<7))) {

        status = status | SPI_Receive(0x07) ;
//        message_buffer[10] = status;
//        message_buffer[11] = status<<1;
//        display_buffer(message_buffer, 0);
        if(status & 0x01) {
            SPI_Transmit(0x05,0x80);            //RX_GO
            status = 0x00;
            timeout=0;// transmission failed.
        }
        if(status & (1<<2)) {
            SPI_Transmit(0x05,0x80);            //RX_GO
            status = 0x00;
            timeout=0;// buffer fail
        } else if (status & 0x02) {
            SPI_Transmit(0x05,0x80);            //RX_GO
            status = 0x00;
            timeout=0;
            unsigned char rxStatus = SPI_Receive(0x08);
//            message_buffer[14] = rxStatus;
//            message_buffer[15] = rxStatus<<1;
//            display_buffer(message_buffer, 0);
            //if error set required bits and continue
            if ((rxStatus & 0x40) || (rxStatus & 0x20) || (rxStatus & 0x10) || (rxStatus & 0x08) || !(!(rxStatus & 0x02) & (rxStatus & 0x01))) {
                SPI_Transmit(0x05,0x80);            //RX_GO
                SPI_Transmit(0x07,0x80);            //Set Receive Overwrite Interrupt Status
            } else {
                if (!(rxStatus & 0x08)){
                    return 1;           // transmission sucseded.
                }
            }
        } else if(timeout >= 10){
            SPI_Transmit(0x05,0x80);            //RX_GO
            status = 0x00;
            timeout=0; // transmission timeout
        } else if (!(status & (1<<6))){  // <don't need 1<<6
            timeout++;// transmission timeout
        } else {
            //timeout++;// transmission timeout
        }
    }

    return 0;
}

/****************************************************
 ** Transmit packet
 *****************************************************/
unsigned char transmitPacket(unsigned char *data) {
    unsigned char status = 0x00;
    unsigned short i = 0;

    while (i < 1000) {
        SPI_Transmit(0x01, 0x10); // TX_LENGTH
        SPI_Transmit(0x02, 0x40); // TX_CTRL_ADR, TX_CLR

        SPI_Transmit(0x20, data[0]);
        SPI_Transmit(0x20, data[1]);
        SPI_Transmit(0x20, data[2]);
        SPI_Transmit(0x20, data[3]);
        SPI_Transmit(0x20, data[4]);
        SPI_Transmit(0x20, data[5]);
        SPI_Transmit(0x20, data[6]);
        SPI_Transmit(0x20, data[7]);
        SPI_Transmit(0x20, data[8]);
        SPI_Transmit(0x20, data[9]);
        SPI_Transmit(0x20, data[10]);
        SPI_Transmit(0x20, data[11]);
        SPI_Transmit(0x20, data[12]);
        SPI_Transmit(0x20, data[13]);
        SPI_Transmit(0x20, data[14]);
        SPI_Transmit(0x20, data[15]);

        SPI_Transmit(0x02, 0x80); // TX_CTRL_ADR, TX_GO. Start transmit

        status = 0x00;

        while (!status) {
            status = status | (SPI_Receive(0x04) & (0x01 | 0x02));
        }
        // if using transaction mode and no error in transmission
        // we need to make second read to check for ack error
        if (status == 0x02) { // TXC=1, TXE=0.
            if (!(SPI_Receive(0x04) & 0x01)) { // check TXE again.
                return 1; // TX succefull! happy days.
            }
        }
        i++;
    }
    return 0;
}



/****************************************************
 ** Check Transceiver Presence
 *****************************************************/
//void checkTransceiver() {
//    unsigned char channel = SPI_Receive(0x00); // Channel ID
//
//    if (channel != 0xFF) { //pullups enabled so one's are read in case of failure
//        //MISO_LOW(); //disable pullup to reduce consumption
//    }
//}

