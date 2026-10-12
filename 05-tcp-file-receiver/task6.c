/* 
 * Connects to the server and receives a BMP file using
 * the custom TCP-over-TCP protocol in ewpdef.h
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "include/ewpdef.h"

/* flag values from the header comments */
#define FLAG_FIN  0x01
#define FLAG_ACK  0x10
#define FLAG_NACK 0x20  /* URG is used as NACK */

#define MAX_PACKETS 2048
#define OUTFILE     "task6_received.bmp"

/* fixed part of the struct (Data[1] is just a placeholder) */
#define HDR_SIZE ((int)(sizeof(struct EWP_TCP_PROTOCOL_TCP) - 1))

/* holds one received packet so we can sort later */
typedef struct {
    unsigned int   uiSeq;
    unsigned short usLen;
    unsigned char *pucData;
    int            bValid;
} PACKETSLOT;

/* keep reading until we have exactly iLen bytes */
static int recvAll(int iSock, unsigned char *pucBuf, int iLen)
{
    int iGot = 0;
    int iRet = 0;

    while (iGot < iLen) {
        iRet = (int)recv(iSock, pucBuf + iGot, iLen - iGot, 0);
        if (iRet <= 0) {
            return -1;
        }
        iGot += iRet;
    }
    return iGot;
}

/* send an ACK or NACK back to the server */
static void sendAck(int iSock, unsigned int uiSeq, int bNack)
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

/* sort packets by sequence number, simple bubble sort */
static void sortPackets(PACKETSLOT *paSlots, int iCount)
{
    PACKETSLOT stTmp;   /* st = struct */
    int        i = 0;
    int        j = 0;

    memset(&stTmp, 0, sizeof(stTmp));

    for (i = 0; i < iCount - 1; i++) {
        for (j = 0; j < iCount - i - 1; j++) {
            if (paSlots[j].uiSeq > paSlots[j+1].uiSeq) {
                stTmp        = paSlots[j];
                paSlots[j]   = paSlots[j+1];
                paSlots[j+1] = stTmp;
            }
        }
    }
}

int main(int iArgc, char *apszArgv[])
{
    char               szServer[64];
    int                iPort      = 0;
    int                iSock      = -1;
    int                i          = 0;
    int                bDone      = 0;
    int                iCount     = 0;

    struct sockaddr_in stAddr;      /* st = struct */
    PACKETSLOT         aSlots[MAX_PACKETS];

    unsigned char      aucHdr[32];
    unsigned char     *pucPacket  = NULL;
    unsigned char     *pucData    = NULL;

    struct EWP_TCP_PROTOCOL_TCP *pPkt = NULL;

    unsigned int       uiSeq      = 0;
    unsigned short     usDataSize = 0;
    unsigned char      ucFlags    = 0;

    FILE              *pfOut      = NULL;

    memset(szServer, 0, sizeof(szServer));
    memset(&stAddr,  0, sizeof(stAddr));
    memset(aSlots,   0, sizeof(aSlots));
    memset(aucHdr,   0, sizeof(aucHdr));

    /* read -server and -port from command line */
    for (i = 1; i < iArgc - 1; i++) {
        if (strcmp(apszArgv[i], "-server") == 0) {
            strncpy(szServer, apszArgv[i+1], sizeof(szServer) - 1);
        } else if (strcmp(apszArgv[i], "-port") == 0) {
            iPort = atoi(apszArgv[i+1]);
        }
    }

    if (szServer[0] == '\0' || iPort == 0) {
        fprintf(stderr, "Usage: ./task6 -server <ip> -port <port>\n");
        return 1;
    }

    /* open socket and connect */
    iSock = socket(AF_INET, SOCK_STREAM, 0);
    if (iSock < 0) {
        fprintf(stderr, "Error: socket() failed\n");
        return 1;
    }

    stAddr.sin_family = AF_INET;
    stAddr.sin_port   = htons((unsigned short)iPort);
    inet_pton(AF_INET, szServer, &stAddr.sin_addr);

    if (connect(iSock, (struct sockaddr *)&stAddr, sizeof(stAddr)) < 0) {
        fprintf(stderr, "Error: connect() failed\n");
        close(iSock);
        return 1;
    }

    printf("Connected to %s:%d\n", szServer, iPort);

    /* main receive loop */
    while (!bDone && iCount < MAX_PACKETS) {

        memset(aucHdr, 0, sizeof(aucHdr));

        /* read the fixed header first */
        if (recvAll(iSock, aucHdr, HDR_SIZE) < 0) {
            printf("Server closed connection\n");
            break;
        }

        pPkt       = (struct EWP_TCP_PROTOCOL_TCP *)aucHdr;
        uiSeq      = pPkt->uiSequenceNumber;
        usDataSize = pPkt->usSizeOfPacket;
        ucFlags    = pPkt->ucFlags;

        printf("seq=%-4u  bytes=%-5u  flags=0x%02x\n",
               uiSeq, usDataSize, ucFlags);

        /* allocate buffer for header + data */
        pucPacket = (unsigned char *)malloc(HDR_SIZE + usDataSize);
        if (pucPacket == NULL) {
            fprintf(stderr, "Error: malloc failed\n");
            break;
        }
        memset(pucPacket, 0, HDR_SIZE + usDataSize);
        memcpy(pucPacket, aucHdr, HDR_SIZE);

        /* read the data payload */
        if (usDataSize > 0) {
            if (recvAll(iSock, pucPacket + HDR_SIZE, usDataSize) < 0) {
                free(pucPacket);
                pucPacket = NULL;
                break;
            }
        }

        /* save the data payload */
        if (usDataSize > 0) {
            pucData = (unsigned char *)malloc(usDataSize);
            if (pucData != NULL) {
                memset(pucData, 0, usDataSize);
                memcpy(pucData, pucPacket + HDR_SIZE, usDataSize);
                aSlots[iCount].uiSeq   = uiSeq;
                aSlots[iCount].usLen   = usDataSize;
                aSlots[iCount].pucData = pucData;
                aSlots[iCount].bValid  = 1;
                iCount++;
            }
        }

        free(pucPacket);
        pucPacket = NULL;

        /* send ACK back to server */
        sendAck(iSock, uiSeq, 0);

        if (ucFlags & FLAG_FIN) {
            printf("Got FIN, transfer done\n");
            bDone = 1;
        }
    }

    close(iSock);

    /* sort and write to file */
    printf("Sorting %d packets...\n", iCount);
    sortPackets(aSlots, iCount);

    pfOut = fopen(OUTFILE, "wb");
    if (pfOut == NULL) {
        fprintf(stderr, "Error: could not open %s for writing\n", OUTFILE);
    } else {
        for (i = 0; i < iCount; i++) {
            if (aSlots[i].bValid && aSlots[i].pucData != NULL) {
                fwrite(aSlots[i].pucData, 1, aSlots[i].usLen, pfOut);
            }
        }
        fclose(pfOut);
        pfOut = NULL;
        printf("File saved as %s\n", OUTFILE);
    }

    /* free everything */
    for (i = 0; i < iCount; i++) {
        if (aSlots[i].pucData != NULL) {
            free(aSlots[i].pucData);
            aSlots[i].pucData = NULL;
        }
    }

    return 0;
}
