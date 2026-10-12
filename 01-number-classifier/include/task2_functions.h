/* 
 Header file for all math check functions and the metadata struct.
*/

#ifndef TASK2_FUNCTIONS_H
#define TASK2_FUNCTIONS_H
#include <stdbool.h>

/* function declarations */
bool isFibonacci(int n);
bool isPrime(int n);
bool isSquareNumber(int n);
bool isCubeNumber(int n);
bool isPerfectNumber(int n);
bool isAbundantNumber(int n);
bool isDeficientNumber(int n);
bool isOdd(int n);

/*
   pragma pack ensures consistent binary size in output file */
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
