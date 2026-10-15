/* 
 Headerfil for alle mattesjekkfunksjonene og metadata-structen.
*/

#ifndef TASK2_FUNCTIONS_H
#define TASK2_FUNCTIONS_H
#include <stdbool.h>

/* funksjonsdeklarasjoner */
bool isFibonacci(int n);
bool isPrime(int n);
bool isSquareNumber(int n);
bool isCubeNumber(int n);
bool isPerfectNumber(int n);
bool isAbundantNumber(int n);
bool isDeficientNumber(int n);
bool isOdd(int n);

/*
   pragma pack sikrer lik binær størrelse i utdatafilen */
#pragma pack(1)
struct TASK2_NUMBERS_METADATA {
    int iIndex;
    int iNumber;
    int bIsFibonacci;
    int bIsPrimeNumber;
    int bIsSquareNumber;
    int bIsCubeNumber;
    int bIsPerfectNumber;
    int bIsAbundantNumber;
    int bIsDeficientNumber;
    int bIsOddNumber;
};
#pragma pack()

#endif
