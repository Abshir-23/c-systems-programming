#ifndef TASK6_H
#define TASK6_H

#include "ewpdef.h"

/* flaggverdier fra protokollheaderen */
#define FLAG_FIN  0x01
#define FLAG_ACK  0x10
#define FLAG_NACK 0x20

#define MAX_PACKETS 2048
#define OUTFILE     "task6_received.bmp"

/* den faste delen av structen (Data[1] er bare en plassholder) */
#define HDR_SIZE ((int)(sizeof(struct EWP_TCP_PROTOCOL_TCP) - 1))

/* holder én mottatt pakke slik at vi kan sortere senere */
typedef struct {
    unsigned int   uiSeq;
    unsigned short usLen;
    unsigned char *pucData;
    int            bValid;
} PACKETSLOT;

/* util.c */
int  recvAll(int iSock, unsigned char *pucBuf, int iLen);
void sortPackets(PACKETSLOT *paSlots, int iCount);

/* client.c */
void sendAck(int iSock, unsigned int uiSeq, int bNack);

#endif
