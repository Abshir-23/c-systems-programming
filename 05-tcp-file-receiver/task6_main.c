/* 
   Utility functions: reliable recv and packet sorting */
#include <string.h>
#include <sys/socket.h>
#include "include/task6.h"
/* keep reading until we have exactly iLen bytes */
int recvAll(int iSock, unsigned char *pucBuf, int iLen)
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
/* sort packets by sequence number, simple bubble sort */
void sortPackets(PACKETSLOT *paSlots, int iCount)
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
