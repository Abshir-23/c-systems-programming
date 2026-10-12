/* 
   Helper functions used by the server */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include "include/task5.h"

/* returns 1 if filename is safe to use, 0 if not.
   blocks path traversal like ../../etc/passwd */
int isValidFilename(const char *pszName)
{
    int i    = 0;
    int iLen = 0;

    if (pszName == NULL || pszName[0] == '\0') return 0;
    if (pszName[0] == '.') return 0;

    iLen = (int)strlen(pszName);
    if (iLen > 50) return 0;

    for (i = 0; i < iLen; i++) {
        char c = pszName[i];
        if (c == '/' || c == '\\') return 0;
        if (!isalnum(c) && c != '.' && c != '_' && c != '-')
            return 0;
    }
    return 1;
}

/* recv() may return less than asked — loop until we have all iLen bytes */
int recvAll(int iSock, char *pBuf, int iLen)
{
    int iTotal = 0;
    int iRcv   = 0;

    while (iTotal < iLen) {
        iRcv = recv(iSock, pBuf + iTotal, iLen - iTotal, 0);
        if (iRcv <= 0) return -1;
        iTotal += iRcv;
    }
    return iTotal;
}
