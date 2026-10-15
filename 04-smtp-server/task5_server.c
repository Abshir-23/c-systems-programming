/*
   Håndtering av EWP-protokollen: sende svar, ta imot filer og håndtere klientkommandoer */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include "include/task5.h"

/* skriver magisk tall, pakkestørrelse og skilletegn inn i EWP-headeren */
void fillHeader(struct EWP_SMTP_PROTOCOL_SIZEHEADER *pHdr, int iTotal)
{
    char szSize[5] = {0};
    memcpy(pHdr->acMagicNumber, "EWP", 3);
    snprintf(szSize, sizeof(szSize), "%04d", iTotal);
    memcpy(pHdr->acDataSize, szSize, 4);
    pHdr->acDelimeter[0] = '|';
}

/* sender et standard statussvar til klienten, f.eks. "250 Sender ok" */
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

/* leser innkommende datapakker og skriver til fil.
   stopper når et punktum alene mottas (SMTP-markør for slutt på melding) */
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

        /* punktum alene = slutt på filen, slutt å lese */
        if ((iPayload == 1 && szPayload[0] == '.') ||
            (iPayload == 2 && szPayload[0] == '.' && szPayload[1] == '\n') ||
            (iPayload == 3 && szPayload[0] == '.' &&
             szPayload[1] == '\r' && szPayload[2] == '\n'))
            break;

        fwrite(szPayload, 1, iPayload, pfOut);
    }
}

/* håndterer én tilkoblet klient gjennom hele den SMTP-lignende sesjonen.
   behandler kommandoene HELO, MAIL, RCPT, DATA og QUIT */
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
    time_t    lNow       = 0;   /* time_t er en long under panseret */
    int       iTotalSize = 0;
    int       iPayload   = 0;
    int       bRunning   = 1;
    struct tm *pTm       = NULL;

    /* send 220-velkomst så snart klienten kobler til */
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
    printf("[->] 220 aksept fra server sendt\n");

    while (bRunning) {

        /* les den 8 byte lange EWP-headeren først */
        memset(&stHead, 0, sizeof(stHead));
        if (recvAll(iClientSock, (char *)&stHead, HEADER_SIZE) < 0) {
            printf("[  ] Klienten koblet fra.\n");
            break;
        }

        {
            char szSize[5] = {0};
            memcpy(szSize, stHead.acDataSize, 4);
            iTotalSize = atoi(szSize);
        }

        iPayload = iTotalSize - HEADER_SIZE;
        if (iPayload <= 0 || iPayload >= 2048) {
            printf("[  ] Ugyldig pakkestørrelse: %d\n", iPayload);
            break;
        }

        /* les innholdet etter headeren */
        memset(szPayload, 0, sizeof(szPayload));
        if (recvAll(iClientSock, szPayload, iPayload) < 0) {
            printf("[  ] Lesing feilet.\n");
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
            printf("[->] 250 HELO-svar\n");

        } else if (strncmp(szPayload, "MAIL", 4) == 0) {

            printf("[<-] MAIL FROM\n");
            sendReply(iClientSock, "250", "Sender ok");

        } else if (strncmp(szPayload, "RCPT", 4) == 0) {

            printf("[<-] RCPT TO\n");
            sendReply(iClientSock, "250", "Recipient ok");

        } else if (strncmp(szPayload, "DATA", 4) == 0) {

            /* hent ut filnavnet fra innholdet "DATA <filnavn>" */
            memset(szFilename, 0, sizeof(szFilename));
            if (iPayload > 5) {
                int iLen = 0;
                strncpy(szFilename, szPayload + 5, sizeof(szFilename) - 1);
                szFilename[sizeof(szFilename) - 1] = '\0';
                iLen = (int)strlen(szFilename);
                /* fjern mellomrom og linjeskift på slutten */
                while (iLen > 0 &&
                       (szFilename[iLen-1] == ' '  ||
                        szFilename[iLen-1] == '\r' ||
                        szFilename[iLen-1] == '\n'))
                    szFilename[--iLen] = '\0';
            }
            printf("[<-] DATA filnavn='%s'\n", szFilename);

            /* avvis ugyldige filnavn før noe åpnes */
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
                    printf("[  ] Lagret: '%s'\n", szFilename);
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

    /* lukk filen hvis løkken avsluttet uventet midt i overføringen */
    if (pfOutput != NULL) {
        fclose(pfOutput);
        pfOutput = NULL;
    }
}
