#include "usbTranceiver.h"

int main() {

    init_ports();
    sei();
    timer16_init();
    spiMasterInit();
    _delay_ms(1000);
    wirelessTransceiverInit();

    unsigned char packet[16];
    unsigned char message[16];
    unsigned char transmissionStatus = 0x00;
    unsigned char receptionStatus = 0x00;

    while (1) {

        setRXmode();
        setTXmode();

        for (unsigned char i = 0; i < 16; i++)
            message[i] = 5;

        for (unsigned char i = 0; i < 16; ++i)
            packet[i] = receiveDataFromFifo();

        sendDataToFifo(77);

        unsigned char numberOfPackets = packet[0];

        setTXmode();
        transmissionStatus = transmitPacket(packet);
        sendDataToFifo(transmissionStatus);

        if (transmissionStatus) {
            //Transmit data packets if there is any
            for (unsigned char j = 0; j < numberOfPackets; j++) {

                for (unsigned char i = 0; i < 16; i++) {
                    packet[i] = receiveDataFromFifo();
                }
                setTXmode();
                transmissionStatus = transmitPacket(packet);
                sendDataToFifo(transmissionStatus);
            }

            //Receive response message
            setRXmode();
            receptionStatus = receivePacket(message);
            sendDataToFifo(packetReceiveTimeout);
            for (unsigned char i = 0; i < 16; i++) {
                sendDataToFifo(message[i]);
            }

            //If response message is 'OK' receive data from MATRIX
            if (message[1] == 'O' && message[2] == 'K') {

                unsigned char numberOfResponsePackets = message[0];

                for (unsigned char j = 0; j < numberOfResponsePackets; j++) {
                    setRXmode();
                    receptionStatus = receivePacket(message);
                    sendDataToFifo(receptionStatus);

                    for (unsigned char i = 0; i < 16; i++) {
                        sendDataToFifo(message[i]);
                    }
                }
            }

            sendDataToFifo(88);

        }

    } // while(1)
}

unsigned char receiveDataFromFifo() {

    unsigned char dataReceived = 0;
    unsigned char fifoData = 0;

    while (!dataReceived) {

        if (!(READ_RXF())) {

            fifoData = 0x00;
            DDRC = 0x00;
            DDRD = 0x60;
            FIFO_RDATA_ENABLE(); //0 falling edge puts data on DX line
            _delay_us(1);

            if (PIND & (1 << PD1)) {
                fifoData = fifoData | 0x01;
            }
            if (PINC & (1 << PC4)) {
                fifoData = fifoData | 0x02;
            }
            if (PINC & (1 << PC5)) {
                fifoData = fifoData | 0x04;
            }
            if (PINC & (1 << PC0)) {
                fifoData = fifoData | 0x08;
            }
            if (PIND & (1 << PD0)) {
                fifoData = fifoData | 0x10;
            }
            if (PINC & (1 << PC2)) {
                fifoData = fifoData | 0x20;
            }
            if (PINC & (1 << PC1)) {
                fifoData = fifoData | 0x40;
            }
            if (PINC & (1 << PC3)) {
                fifoData = fifoData | 0x80;
            }

            FIFO_RDATA_DISABLE(); //1 always keep RD high when inactive

            dataReceived = 1;
        }

    }

    return fifoData;
}

void sendDataToFifo(unsigned char fifoData) {

    unsigned char dataSent = 0;

    while (!dataSent) {

        if (!(READ_TXE())) {

            DDRC = 0x3F;
            DDRD = 0x63;
            if (fifoData & 0x01)
                D0_EN();
            else
                D0_DIS();
            if (fifoData & 0x02)
                D1_EN();
            else
                D1_DIS();
            if (fifoData & 0x04)
                D2_EN();
            else
                D2_DIS();
            if (fifoData & 0x08)
                D3_EN();
            else
                D3_DIS();
            if (fifoData & 0x10)
                D4_EN();
            else
                D4_DIS();
            if (fifoData & 0x20)
                D5_EN();
            else
                D5_DIS();
            if (fifoData & 0x40)
                D6_EN();
            else
                D6_DIS();
            if (fifoData & 0x80)
                D7_EN();
            else
                D7_DIS();

            FIFO_WDATA_DISABLE(); //1 make WR high
            _delay_us(1);
            FIFO_WDATA_ENABLE(); //0 falling edge puts data on transmit buffer

            //FIFO_WDATA_DISABLE(); //1 make WR high

            dataSent = 1;
        }

    }

}

/****************************************************
 ** Initializes 16 bit timer
 *****************************************************/
void timer16_init(void) {
    // write high byte first for 16 bit register access:
    TCNT1H = 0; // set counter to zero
    TCNT1L = 0;
    // Mode 4 table 14-4 page 132. CTC mode and top in OCR1A
    // WGM13=0, WGM12=1, WGM11=0, WGM10=0
    TCCR1A = (0 << COM1B1) | (0 << COM1B0) | (0 << WGM11);

    if (F_CPU == 8000000UL) {
        TCCR1B = (1 << CS12) | (1 << CS10) | (1 << WGM12) | (0 << WGM13); // crystal clock/1024
        // At what value to cause interrupt. You can use this for calibration
        // of the clock. Theoretical value for 20MHz: 19531=0x4C and 0x4B
        OCR1AH = 0x1E;
        OCR1AL = 0x84;
    }

    // interrupt mask bit:
    TIMSK = (1 << OCIE1A);
}

//###################################################
//############ INTERRUPTS ###########################
//###################################################

ISR(TIMER1_COMPA_vect)
{

    packetReceiveTimeout++;
    if (packetReceiveTimeout > 8) {
        packetReceiveTimeout = 0;
    }

}

/****************************************************
 ** Initializes External Interrupt
 *****************************************************/

//void externalInterruptInit() {

 //   EICRA = (1 << ISC11) | (1 << ISC10);
 //   EIMSK = (1 << INT0);
//}

/****************************************************
 ** Initializes ports
 *****************************************************/
void init_ports() {

    //	0		1		2		3		4		5		6		7
    //	D3		D6		D5		D7		D1		D2		NA		NA
    //	0		0		0		0		0		0		0		0
    DDRC = 0x00;
    PORTC = 0x00;
    //	0		1		2		3		4		5		6		7
    //	RST		PACTL	SS		MOSI	MISO	SCK		XTAL	XTAL
    //	1		0		1		1		0		1		0		0
    DDRB = 0x2D;
    PORTB = 0x10;
    //	0		1		2		3		4		5		6		7
    //	D4		D0		IRQ		RXF		TXE		WR		RD		PRWREN
    //	0		0		0		0		0		1		1		0
    DDRD = 0x60;
    PORTD = 0x9C;
}

