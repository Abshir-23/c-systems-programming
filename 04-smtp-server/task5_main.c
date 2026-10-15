/* 
   Startpunkt. Tolker argumentene, setter opp TCP-socket og tar imot én klient.
    */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "include/task5.h"

/* leser -port og -id fra argv.
   returnerer 1 hvis begge finnes, 0 hvis de mangler */
int parseArgs(int iArgc, char *apszArgv[],
              int *piPort, char *pszId, int iIdMax)
{
    int i = 0;
    *piPort  = 0;
    pszId[0] = '\0';

    for (i = 1; i < iArgc - 1; i++) {
        if (strcmp(apszArgv[i], "-port") == 0)
            *piPort = atoi(apszArgv[i + 1]);
        else if (strcmp(apszArgv[i], "-id") == 0) {
            strncpy(pszId, apszArgv[i + 1], iIdMax - 1);
            pszId[iIdMax - 1] = '\0';
        }
    }
    return (*piPort > 0 && pszId[0] != '\0') ? 1 : 0;
}

int main(int iArgc, char *apszArgv[])
{
    int                iPort       = 0;
    char               szId[64]    = {0};
    int                iServerSock = 0;
    int                iClientSock = 0;
    int                iOpt        = 1;
    struct sockaddr_in stServer;
    struct sockaddr_in stClient;
    socklen_t          iClientLen  = sizeof(stClient);
    char               szClientIp[INET_ADDRSTRLEN];

    if (!parseArgs(iArgc, apszArgv, &iPort, szId, sizeof(szId))) {
        printf("Bruk: ./task5 -port <port> -id <id>\n");
        return 1;
    }

    printf("\n*** SMTP-server ***\n");
    printf("- ID   : %s\n", szId);
    printf("- Port : %d\n\n", iPort);

    iServerSock = socket(AF_INET, SOCK_STREAM, 0);
    if (iServerSock < 0) {
        perror("[ERROR] socket");
        return 1;
    }

    /* gjør at serveren kan startes på nytt med en gang uten å vente på tidsavbrudd i OS */
    setsockopt(iServerSock, SOL_SOCKET, SO_REUSEADDR, &iOpt, sizeof(iOpt));

    memset(&stServer, 0, sizeof(stServer));
    stServer.sin_family      = AF_INET;
    stServer.sin_addr.s_addr = inet_addr("127.0.0.1");
    stServer.sin_port        = htons((unsigned short)iPort);

    if (bind(iServerSock, (struct sockaddr *)&stServer, sizeof(stServer)) < 0) {
        perror("[ERROR] bind");
        close(iServerSock);
        return 1;
    }

    if (listen(iServerSock, 5) < 0) {
        perror("[ERROR] listen");
        close(iServerSock);
        return 1;
    }

    printf("- Lytter på 127.0.0.1:%d ...\n\n", iPort);

    /* vent her til en klient kobler til */
    iClientSock = accept(iServerSock,
                         (struct sockaddr *)&stClient,
                         &iClientLen);
    if (iClientSock < 0) {
        perror("[ERROR] accept");
        close(iServerSock);
        return 1;
    }

    inet_ntop(AF_INET, &stClient.sin_addr, szClientIp, INET_ADDRSTRLEN);
    printf("- Klient tilkoblet: %s\n\n", szClientIp);

    handleClient(iClientSock, szClientIp, szId);

    close(iClientSock);
    close(iServerSock);

    puts("\n*** Ferdig. Ha det! ***\n");
    return 0;
}
