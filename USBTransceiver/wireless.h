#include "global.h"
#include "spi.h"

//unsigned char transmitPacket(unsigned char *data);
unsigned char receivePacket(unsigned char *packet);
void setRXmode();
void setTXmode();
void wirelessTransceiverInit();
unsigned char receivePacket(unsigned char * packet);
unsigned char transmitPacket(unsigned char *data);
