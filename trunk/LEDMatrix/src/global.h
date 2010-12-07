#ifndef GLOBAL_H_
#define GLOBAL_H_

extern unsigned char packetReceiveTimeout;
extern unsigned char rssi;
extern unsigned char message_buffer[80];
extern unsigned char configuration[16];
/*
 * [0] = numberOfPackets
 * [1] = time
 * [2] = time
 * [3] = time
 * [4] = time
 * [5] = time
 * [6] = time
 * [7] = speed
 * ...
 */
extern unsigned char FALSE;
extern unsigned char TRUE;

#endif /* GLOBAL_H_ */
