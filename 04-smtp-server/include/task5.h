/* 
   Forhåndsdeklarasjoner for alle funksjonene i task5 */

#ifndef TASK5_H
#define TASK5_H

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "ewpdef.h"

#define HEADER_SIZE 8

/* task5_utils.c */
int  isValidFilename(const char *pszName);
int  recvAll(int iSock, char *pBuf, int iLen);

/* task5_server.c */
void fillHeader(struct EWP_SMTP_PROTOCOL_SIZEHEADER *pHdr, int iTotal);
void sendReply(int iSock, const char *pszCode, const char *pszMsg);
void recvFileData(int iSock, FILE *pfOut);
void handleClient(int iClientSock, const char *pszClientIp, const char *pszServerId);

/* task5_main.c */
int parseArgs(int iArgc, char *apszArgv[], int *piPort, char *pszId, int iIdMax);

#endif
