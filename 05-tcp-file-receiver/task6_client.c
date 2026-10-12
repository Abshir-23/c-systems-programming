/*
   Client communication: sends ACK or NACK back to server */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "include/task6.h"

/* send an ACK or NACK back to the server */
void sendAck(int iSock, unsigned int uiSeq, int bNack)
{
    struct EWP_TCP_PROTOCOL_TCP *pPkt    = NULL;
    unsigned char                        *pucBuf  = NULL;

    pucBuf = (unsigned char *)malloc(HDR_SIZE);
    if (pucBuf == NULL) {
        return;
    }
    memset(pucBuf, 0, HDR_SIZE);

    pPkt = (struct EWP_TCP_PROTOCOL_TCP *)pucBuf;
    pPkt->uiAckNumber    = uiSeq;
    pPkt->ucDataOffset   = 5;
    pPkt->ucFlags        = bNack ? FLAG_NACK : FLAG_ACK;
    pPkt->usSizeOfPacket = 0;
    pPkt->usChecksum     = 0;  /* checksum not used in ACK response */

    send(iSock, pucBuf, HDR_SIZE, 0);

    free(pucBuf);
    pucBuf = NULL;
}
