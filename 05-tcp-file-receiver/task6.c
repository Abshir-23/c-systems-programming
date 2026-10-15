/* 
 * Kobler til serveren og tar imot en BMP-fil med
 * den egne TCP-over-TCP-protokollen i ewpdef.h
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "include/ewpdef.h"

/* flaggverdier fra kommentarene i headerfilen */
#define FLAG_FIN  0x01
#define FLAG_ACK  0x10
#define FLAG_NACK 0x20  /* URG brukes som NACK */

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

/* fortsett å lese til vi har nøyaktig iLen byte */
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

/* send ACK eller NACK tilbake til serveren */
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
    pPkt->usChecksum     = 0;  /* sjekksum brukes ikke i ACK-svaret */

    send(iSock, pucBuf, HDR_SIZE, 0);

    free(pucBuf);
    pucBuf = NULL;
}

/* sorter pakkene etter sekvensnummer, enkel boblesortering */
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

    /* les -server og -port fra kommandolinjen */
    for (i = 1; i < iArgc - 1; i++) {
        if (strcmp(apszArgv[i], "-server") == 0) {
            strncpy(szServer, apszArgv[i+1], sizeof(szServer) - 1);
        } else if (strcmp(apszArgv[i], "-port") == 0) {
            iPort = atoi(apszArgv[i+1]);
        }
    }

    if (szServer[0] == '\0' || iPort == 0) {
        fprintf(stderr, "Bruk: ./task6 -server <ip> -port <port>\n");
        return 1;
    }

    /* åpne socket og koble til */
    iSock = socket(AF_INET, SOCK_STREAM, 0);
    if (iSock < 0) {
        fprintf(stderr, "Feil: socket() feilet\n");
        return 1;
    }

    stAddr.sin_family = AF_INET;
    stAddr.sin_port   = htons((unsigned short)iPort);
    inet_pton(AF_INET, szServer, &stAddr.sin_addr);

    if (connect(iSock, (struct sockaddr *)&stAddr, sizeof(stAddr)) < 0) {
        fprintf(stderr, "Feil: connect() feilet\n");
        close(iSock);
        return 1;
    }

    printf("Koblet til %s:%d\n", szServer, iPort);

    /* hovedløkken for mottak */
    while (!bDone && iCount < MAX_PACKETS) {

        memset(aucHdr, 0, sizeof(aucHdr));

        /* les den faste headeren først */
        if (recvAll(iSock, aucHdr, HDR_SIZE) < 0) {
            printf("Serveren lukket forbindelsen\n");
            break;
        }

        pPkt       = (struct EWP_TCP_PROTOCOL_TCP *)aucHdr;
        uiSeq      = pPkt->uiSequenceNumber;
        usDataSize = pPkt->usSizeOfPacket;
        ucFlags    = pPkt->ucFlags;

        printf("sekv=%-4u  byte=%-5u  flagg=0x%02x\n",
               uiSeq, usDataSize, ucFlags);

        /* alloker buffer til header + data */
        pucPacket = (unsigned char *)malloc(HDR_SIZE + usDataSize);
        if (pucPacket == NULL) {
            fprintf(stderr, "Feil: malloc feilet\n");
            break;
        }
        memset(pucPacket, 0, HDR_SIZE + usDataSize);
        memcpy(pucPacket, aucHdr, HDR_SIZE);

        /* les innholdet */
        if (usDataSize > 0) {
            if (recvAll(iSock, pucPacket + HDR_SIZE, usDataSize) < 0) {
                free(pucPacket);
                pucPacket = NULL;
                break;
            }
        }

        /* lagre innholdet */
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

        /* send ACK tilbake til serveren */
        sendAck(iSock, uiSeq, 0);

        if (ucFlags & FLAG_FIN) {
            printf("Mottok FIN, overføringen er ferdig\n");
            bDone = 1;
        }
    }

    close(iSock);

    /* sorter og skriv til fil */
    printf("Sorterer %d pakker...\n", iCount);
    sortPackets(aSlots, iCount);

    pfOut = fopen(OUTFILE, "wb");
    if (pfOut == NULL) {
        fprintf(stderr, "Feil: kunne ikke åpne %s for skriving\n", OUTFILE);
    } else {
        for (i = 0; i < iCount; i++) {
            if (aSlots[i].bValid && aSlots[i].pucData != NULL) {
                fwrite(aSlots[i].pucData, 1, aSlots[i].usLen, pfOut);
            }
        }
        fclose(pfOut);
        pfOut = NULL;
        printf("Filen er lagret som %s\n", OUTFILE);
    }

    /* frigjør alt */
    for (i = 0; i < iCount; i++) {
        if (aSlots[i].pucData != NULL) {
            free(aSlots[i].pucData);
            aSlots[i].pucData = NULL;
        }
    }

    return 0;
}
