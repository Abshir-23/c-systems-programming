/*
   EWP protocol handling: send replies, receive files, handle client commands */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include "include/task5.h"

/* writes magic number, packet size and delimiter into the EWP header */
void fillHeader(struct EWP_SMTP_PROTOCOL_SIZEHEADER *pHdr, int iTotal)
{
    char szSize[5] = {0};
    memcpy(pHdr->acMagicNumber, "EWP", 3);
    snprintf(szSize, sizeof(szSize), "%04d", iTotal);
    memcpy(pHdr->acDataSize, szSize, 4);
    pHdr->acDelimeter[0] = '|';
}

/* sends a standard status reply to the client, e.g. "250 Sender ok" */
void sendReply(int iSock, const char *pszCode, const char *pszMsg)
{
    struct EWP_SMTP_PROTOCOL_SERVERREPLY stReply;
    memset(&stReply, 0, sizeof(stReply));
    fillHeader(&stReply.stHead, (int)sizeof(stReply));
    memcpy(stReply.acStatusCode, pszCode, 3);
    stReply.acHardSpace[0] = ' ';
    strncpy(stReply.acFormattedString, pszMsg, 50);
    stReply.acFormattedString[50] = '\0';
    stReply.acHardZero[0] = '\0';
    send(iSock, &stReply, sizeof(stReply), 0);
    printf("[->] %s %s\n", pszCode, pszMsg);
}

/* reads incoming data packets and writes to file.
   stops when a lone dot is received (SMTP end-of-message marker) */
void recvFileData(int iSock, FILE *pfOut)
{
    struct EWP_SMTP_PROTOCOL_SIZEHEADER stHead;
    char szPayload[2048];
    int  iTotalSize = 0;
    int  iPayload   = 0;

    for (;;) {
        memset(&stHead, 0, sizeof(stHead));
        if (recvAll(iSock, (char *)&stHead, HEADER_SIZE) < 0) return;

        {
            char szSize[5] = {0};
            memcpy(szSize, stHead.acDataSize, 4);
            iTotalSize = atoi(szSize);
        }

        iPayload = iTotalSize - HEADER_SIZE;
        if (iPayload <= 0 || iPayload >= 2048) return;

        memset(szPayload, 0, sizeof(szPayload));
        if (recvAll(iSock, szPayload, iPayload) < 0) return;

        /* lone dot = end of file, stop reading */
        if ((iPayload == 1 && szPayload[0] == '.') ||
            (iPayload == 2 && szPayload[0] == '.' && szPayload[1] == '\n') ||
            (iPayload == 3 && szPayload[0] == '.' &&
             szPayload[1] == '\r' && szPayload[2] == '\n'))
            break;

        fwrite(szPayload, 1, iPayload, pfOut);
    }
}

/* handles one connected client through the full SMTP-like session.
   processes HELO, MAIL, RCPT, DATA and QUIT commands */
void handleClient(int iClientSock,
                  const char *pszClientIp,
                  const char *pszServerId)
{
    struct EWP_SMTP_PROTOCOL_SERVERACCEPT stAccept;
    struct EWP_SMTP_PROTOCOL_SERVERHELO   stHelo;
    struct EWP_SMTP_PROTOCOL_SIZEHEADER   stHead;
    char      szPayload[2048];
    char      szFilename[64];
    char      szTimestamp[32];
    FILE     *pfOutput   = NULL;
    time_t    lNow       = 0;   /* time_t is long under the hood */
    int       iTotalSize = 0;
    int       iPayload   = 0;
    int       bRunning   = 1;
    struct tm *pTm       = NULL;

    /* send 220 welcome as soon as client connects */
    memset(&stAccept, 0, sizeof(stAccept));
    fillHeader(&stAccept.stHead, (int)sizeof(stAccept));
    memcpy(stAccept.acStatusCode, "220", 3);
    stAccept.acHardSpace[0] = ' ';
    lNow = time(NULL);
    pTm  = localtime(&lNow);
    strftime(szTimestamp, sizeof(szTimestamp), "%d %b %Y, %H:%M:%S", pTm);
    snprintf(stAccept.acFormattedString,
             sizeof(stAccept.acFormattedString),
             "127.0.0.1 SMTP %.10s %.18s", pszServerId, szTimestamp);
    stAccept.acHardZero[0] = '\0';
    send(iClientSock, &stAccept, sizeof(stAccept), 0);
    printf("[->] 220 server accept sent\n");

    while (bRunning) {

        /* read the 8-byte EWP header first */
        memset(&stHead, 0, sizeof(stHead));
        if (recvAll(iClientSock, (char *)&stHead, HEADER_SIZE) < 0) {
            printf("[  ] Client disconnected.\n");
            break;
        }

        {
            char szSize[5] = {0};
            memcpy(szSize, stHead.acDataSize, 4);
            iTotalSize = atoi(szSize);
        }

        iPayload = iTotalSize - HEADER_SIZE;
        if (iPayload <= 0 || iPayload >= 2048) {
            printf("[  ] Bad packet size: %d\n", iPayload);
            break;
        }

        /* read the payload after the header */
        memset(szPayload, 0, sizeof(szPayload));
        if (recvAll(iClientSock, szPayload, iPayload) < 0) {
            printf("[  ] Read failed.\n");
            break;
        }

        if (strncmp(szPayload, "HELO", 4) == 0) {

            printf("[<-] HELO\n");
            memset(&stHelo, 0, sizeof(stHelo));
            fillHeader(&stHelo.stHead, (int)sizeof(stHelo));
            memcpy(stHelo.acStatusCode, "250", 3);
            stHelo.acHardSpace[0] = ' ';
            snprintf(stHelo.acFormattedString,
                     sizeof(stHelo.acFormattedString),
                     "%s Hello client", pszClientIp);
            stHelo.acHardZero[0] = '\0';
            send(iClientSock, &stHelo, sizeof(stHelo), 0);
            printf("[->] 250 HELO reply\n");

        } else if (strncmp(szPayload, "MAIL", 4) == 0) {

            printf("[<-] MAIL FROM\n");
            sendReply(iClientSock, "250", "Sender ok");

        } else if (strncmp(szPayload, "RCPT", 4) == 0) {

            printf("[<-] RCPT TO\n");
            sendReply(iClientSock, "250", "Recipient ok");

        } else if (strncmp(szPayload, "DATA", 4) == 0) {

            /* extract filename from "DATA <filename>" payload */
            memset(szFilename, 0, sizeof(szFilename));
            if (iPayload > 5) {
                int iLen = 0;
                strncpy(szFilename, szPayload + 5, sizeof(szFilename) - 1);
                szFilename[sizeof(szFilename) - 1] = '\0';
                iLen = (int)strlen(szFilename);
                /* strip trailing spaces and newlines */
                while (iLen > 0 &&
                       (szFilename[iLen-1] == ' '  ||
                        szFilename[iLen-1] == '\r' ||
                        szFilename[iLen-1] == '\n'))
                    szFilename[--iLen] = '\0';
            }
            printf("[<-] DATA filename='%s'\n", szFilename);

            /* reject bad filenames before opening anything */
            if (!isValidFilename(szFilename)) {
                sendReply(iClientSock, "501", "Invalid filename");
            } else {
                pfOutput = fopen(szFilename, "wb");
                if (pfOutput == NULL) {
                    sendReply(iClientSock, "501", "Could not create file");
                } else {
                    sendReply(iClientSock, "354", "Ready for message");
                    recvFileData(iClientSock, pfOutput);
                    fclose(pfOutput);
                    pfOutput = NULL;
                    printf("[  ] Saved: '%s'\n", szFilename);
                    sendReply(iClientSock, "250", "Message accepted");
                }
            }

        } else if (strncmp(szPayload, "QUIT", 4) == 0) {

            printf("[<-] QUIT\n");
            sendReply(iClientSock, "221", "Goodbye");
            sleep(1);
            bRunning = 0;
        }
    }

    /* close file if loop exited unexpectedly mid-transfer */
    if (pfOutput != NULL) {
        fclose(pfOutput);
        pfOutput = NULL;
    }
}
