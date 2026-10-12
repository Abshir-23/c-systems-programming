/* Based on a provided starter file. Fixed and extended by me: mutex synchronisation, explicit initialisation, safer file handling and comments. */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "task4_prime.h"

/* forward declarations - avoids including .c files directly */
void tean(unsigned int *const v, unsigned int *const w,
          const unsigned int *const k, int N);
unsigned int tcp_checksum(const unsigned char *data, size_t length);

typedef struct PGPRIMENUMBER {
   unsigned int          uiPrime;
   struct PGPRIMENUMBER* pNext;
} PGPRIMENUMBER;

/* all shared resources are passed via this struct instead of globals */
struct THREAD_ARGS {
   FILE*                  pfInputFile;   /* pf = pointer to FILE */
   struct PGPRIMENUMBER** pPrimeList;
   pthread_mutex_t*       pMutex;
   int*                   pFound;
};

/* try each prime as a decryption key, save the file if it works */
int tryDecrypt(unsigned int uiPrime) {
   FILE*          pCode   = NULL;
   FILE*          pPlain  = NULL;
   FILE*          pHash   = NULL;
   unsigned char* pBuffer = NULL;
   unsigned char* pOut    = NULL;
   unsigned int   aKey[4] = {0};
   unsigned int   aV[2]   = {0};
   unsigned int   aW[2]   = {0};
   unsigned int   uiCheck = 0;
   int            iResult = 0;
   long           lSize   = 0;
   size_t         i       = 0;

   pCode = fopen("task4_code.bin", "rb");
   if (pCode == NULL) return 0;

   fseek(pCode, 0, SEEK_END);
   lSize = ftell(pCode);
   fseek(pCode, 0, SEEK_SET);

   pBuffer = (unsigned char*)malloc(lSize);
   if (pBuffer == NULL) {
      fclose(pCode); pCode = NULL;
      return 0;
   }
   memset(pBuffer, 0, lSize);

   pOut = (unsigned char*)malloc(lSize);
   if (pOut == NULL) {
      free(pBuffer); pBuffer = NULL;
      fclose(pCode); pCode = NULL;
      return 0;
   }
   memset(pOut, 0, lSize);

   fread(pBuffer, 1, lSize, pCode);
   fclose(pCode);
   pCode = NULL;

   aKey[0] = aKey[1] = aKey[2] = aKey[3] = uiPrime;

   /* decrypt 8 bytes at a time, N=-32 for decrypt mode */
   for (i = 0; i + 8 <= (size_t)lSize; i += 8) {
      memcpy(aV, pBuffer + i, 8);
      tean(aV, aW, aKey, -32);
      memcpy(pOut + i, aW, 8);
   }

   if (memcmp(pOut, "BENGT", 5) == 0) {
      printf("Key found! Prime: %u\n", uiPrime);

      pPlain = fopen("task4_plain.txt", "wb");
      if (pPlain != NULL) {
         fwrite(pOut, 1, lSize, pPlain);
         fclose(pPlain); pPlain = NULL;
      }

      uiCheck = tcp_checksum(pOut, lSize);
      pHash = fopen("task4_plain.hash", "w");
      if (pHash != NULL) {
         fprintf(pHash, "%u\n", uiCheck);
         fclose(pHash); pHash = NULL;
      }
      iResult = 1;
   }

   free(pBuffer); pBuffer = NULL;
   free(pOut);    pOut    = NULL;
   return iResult;
}

void* threadFunction(void* arg) {
   struct THREAD_ARGS*   pArgs          = (struct THREAD_ARGS*)arg;
   unsigned int          auiNumbers[10] = {0};
   int                   iNumbersRead   = 0;
   int                   iIndex         = 0;
   struct PGPRIMENUMBER* pNewNode       = NULL;

   while (1) {

      /* stop if other thread already found the key */
      pthread_mutex_lock(pArgs->pMutex);
      if (*(pArgs->pFound)) {
         pthread_mutex_unlock(pArgs->pMutex);
         break;
      }
      pthread_mutex_unlock(pArgs->pMutex);

      iNumbersRead = 0;

      /* MUTEX LOCK: both threads share the file pointer.
         without this they could read the same numbers twice */
      pthread_mutex_lock(pArgs->pMutex);
      while (iNumbersRead < 10) {
         if (fscanf(pArgs->pfInputFile, "%u",
             &(auiNumbers[iNumbersRead])) == 1)
            iNumbersRead++;
         else
            break;
      }
      pthread_mutex_unlock(pArgs->pMutex);

      if (iNumbersRead == 0) break;

      for (iIndex = 0; iIndex < iNumbersRead; iIndex++) {
         if (isPrime(auiNumbers[iIndex])) {

            pNewNode = (struct PGPRIMENUMBER*)malloc(
                        sizeof(struct PGPRIMENUMBER));
            if (pNewNode == NULL) continue;
            memset(pNewNode, 0, sizeof(struct PGPRIMENUMBER));
            pNewNode->uiPrime = auiNumbers[iIndex];

            /* MUTEX LOCK: both threads share the prime list.
               without this the list pointers could get corrupted */
            pthread_mutex_lock(pArgs->pMutex);
            pNewNode->pNext      = *(pArgs->pPrimeList);
            *(pArgs->pPrimeList) = pNewNode;
            pNewNode             = NULL;
            pthread_mutex_unlock(pArgs->pMutex);

            /* try this prime as decryption key */
            if (tryDecrypt(auiNumbers[iIndex])) {
               pthread_mutex_lock(pArgs->pMutex);
               *(pArgs->pFound) = 1;
               pthread_mutex_unlock(pArgs->pMutex);
               return NULL;
            }
         }
      }
   }
   return NULL;
}

int main(int iArgc, char* apszArgv[]) {
   struct PGPRIMENUMBER* pPrimeList  = NULL;
   FILE*                 pfInputFile = NULL;   /* pf = pointer to FILE */
   pthread_t             tThread1;             /* t = thread type */
   pthread_t             tThread2;
   pthread_mutex_t       tMutex;               /* t = mutex type */
   struct THREAD_ARGS    tArgs;
   struct PGPRIMENUMBER* pPtr   = NULL;
   int                   iFound = 0;
   int                   iRc    = 0;           /* iRc = int return code */

   if (iArgc != 2) {
      printf("ERROR: Usage: ./task4 <filename>\n");
      return 1;
   }

   pfInputFile = fopen(apszArgv[1], "r");
   if (pfInputFile == NULL) {
      printf("ERROR: Could not open file: %s\n", apszArgv[1]);
      return 1;
   }

   /* explicit mutex init as required by the task */
   iRc = pthread_mutex_init(&tMutex, NULL);
   if (iRc != 0) {
      fclose(pfInputFile); pfInputFile = NULL;
      return 1;
   }

   tArgs.pfInputFile = pfInputFile;
   tArgs.pPrimeList  = &pPrimeList;
   tArgs.pMutex      = &tMutex;
   tArgs.pFound      = &iFound;

   iRc = pthread_create(&tThread1, NULL, threadFunction, (void*)&tArgs);
   if (iRc != 0) {
      pthread_mutex_destroy(&tMutex);
      fclose(pfInputFile); pfInputFile = NULL;
      return 1;
   }

   iRc = pthread_create(&tThread2, NULL, threadFunction, (void*)&tArgs);
   if (iRc != 0) {
      pthread_join(tThread1, NULL);
      pthread_mutex_destroy(&tMutex);
      fclose(pfInputFile); pfInputFile = NULL;
      return 1;
   }

   pthread_join(tThread1, NULL);
   pthread_join(tThread2, NULL);

   fclose(pfInputFile);
   pfInputFile = NULL;
   pthread_mutex_destroy(&tMutex);

   printf("\r\nPrime numbers found:\r\n");
   pPtr = pPrimeList;
   while (pPtr != NULL) {
      printf("%u\r\n", pPtr->uiPrime);
      pPtr = pPtr->pNext;
   }

   /* free list node by node */
   while (pPrimeList != NULL) {
      pPtr       = pPrimeList;
      pPrimeList = pPrimeList->pNext;
      free(pPtr);
      pPtr = NULL;
   }

   return 0;
}
