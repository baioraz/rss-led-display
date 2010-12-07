#include "wireless.h"

//#define TXC			0x02
//#define TXE			0x01
//#define RXC			0x02
//#define RXE			0x01
#define badCRC		0x08

// transition cyrf6936 into receive mode
void setRXmode() {
    // set up RX.
    SPI_Transmit(0x05, 0x80); // set RX_CTRL_ADR, RX_GO
    SPI_Transmit(0x0F, 0x13); // XACT_CFG,END_STATE,ACK EN, ACK_TO_15X
    //SPI_Transmit(0x0F,0x93);		// XACT_CFG,END_STATE,ACK EN, ACK_TO_15X
    //SPI_Transmit(0x0F,0xB3);		// XACT_CFG,END_STATE,ACK EN, ACK_TO_15X

    return;
}

// transition cyrf6936 into transmit mode
void setTXmode() {
    // set up TX. (FRC_END cancels receive mode.)

    SPI_Transmit(0x0F, 0x27);
    //	SPI_Transmit(0x0F,0x87);	// XACT_CFG:FRC_END,END_STATE,ACK EN, ACK_TO_15X
    //SPI_Transmit(0x0F,0xA7);	// XACT_CFG:FRC_END,END_STATE,ACK EN, ACK_TO_15X

    return;
}

void wirelessTransceiverInit() {
    SPI_Transmit(0x1D, 0x01); // MODE_OVERRIDE, RST (Reset module)

    //  SPI_Transmit(0x03,0x2F);    //Power Level +4dBm, data mode 8DR, data code length 64 chip codes. Change to 0F for 32 chip codes
    SPI_Transmit(0x03, 0x0F); //Power Level +4dBm, data mode 8DR, data code length 32 chip codes. Change to 0F for 32 chip codes

    SPI_Transmit(0x06, 0x4A); //
    SPI_Transmit(0x0C, 0xC4); //Set aditional pin as GPIO
    //  SPI_Transmit(0x0D,0x10);    //Set aditional pin as GPIO
    SPI_Transmit(0x0F, 0x27); //Ack enable, Ack 15x, idle mode
    //SPI_Transmit(0x10,0xEE);  //Use A4 for 32 chips sop code lenght
    SPI_Transmit(0x10, 0xA4); //Use A4 for 32 chips sop code lenght

    SPI_Transmit(0x11, 0x05); //DATA32_THOLD
    SPI_Transmit(0x12, 0x0E); //DATA64_THOLD
    SPI_Transmit(0x15, 0x14); //CRC LSB
    SPI_Transmit(0x16, 0x14); //CRC MSB
    SPI_Transmit(0x1B, 0x55); //TX_OFFSET_LSB
    SPI_Transmit(0x1C, 0x05); //TX_OFFSET_MSB

    SPI_Transmit(0x26, 0x08); // XTAL_CFG  (must)
    //SPI_Transmit(0x28,0x02);      // (must)
    SPI_Transmit(0x32, 0x3C); // AUTO_CAL_TIME  (must)
    SPI_Transmit(0x35, 0x14); // AUTO_CAL_OFFSET  (must)
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

unsigned char checkReceptionStatus() {

    unsigned char status = 0;
    unsigned char timeout = 0;

    if (!(SPI_Receive(0x05) & (1 << 7))) {

        status = status | SPI_Receive(0x07);

        if (status & 0x01) {
            SPI_Transmit(0x05, 0x80); //RX_GO
            status = 0x00;
            timeout = 0;// transmission failed.
        }
        if (status & (1 << 2)) {
            SPI_Transmit(0x05, 0x80); //RX_GO
            status = 0x00;
            timeout = 0;// buffer fail
        } else if (status & 0x02) {
            SPI_Transmit(0x05, 0x80); //RX_GO
            status = 0x00;
            timeout = 0;
            unsigned char rxStatus = SPI_Receive(0x08);
            //if error set required bits and continue
            if ((rxStatus & 0x40) || (rxStatus & 0x20) || (rxStatus & 0x10) || (rxStatus & 0x08) || !(!(rxStatus & 0x02) & (rxStatus & 0x01))) {
                SPI_Transmit(0x05, 0x80); //RX_GO
                SPI_Transmit(0x07, 0x80); //Set Receive Overwrite Interrupt Status
            } else {

                if (!(rxStatus & 0x08)) {
                    return 1; // transmission sucseded.
                }
            }
        } else if (timeout >= 10) {
            SPI_Transmit(0x05, 0x80); //RX_GO
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

