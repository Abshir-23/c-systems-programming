/* 

   This program reads numbers from a text file, runs each number
   through 8 math functions, and saves the results to a binary file.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/task2_functions.h"

int main(int iArgc, char* apszArgv[]) {

    FILE* pfInput  = NULL;
    FILE* pfOutput = NULL;
    int iNumber = 0;
    int iIndex  = 0;
    struct TASK2_NUMBERS_METADATA* pEntry = NULL;

    /* check args */
    if (iArgc != 2) {
        printf("ERROR: Wrong parameters - Use following input on start:\n");
        printf("        $ ./task2 <filename>\n");
        printf("Try again. Closing program...\n");
        return 1;
    }

    puts("\n*** Number Classifier ***\n");

    /* open input file */
    pfInput = fopen(apszArgv[1], "r");
    if (pfInput == NULL) {
        printf("- ERROR: Could not open input file: %s\n", apszArgv[1]);
        return 1;
    }
    printf("- Opened input file: \"%s\"\n", apszArgv[1]);

    /* open output file for binary writing */
    pfOutput = fopen("output.bin", "wb");
    if (pfOutput == NULL) {
        printf("- ERROR: Could not open output file\n");
        fclose(pfInput);
        pfInput = NULL;
        return 1;
    }
    printf("- Opened output file: \"output.bin\"\n\n");

    /* print table header */
    printf("%-5s %-8s %-5s %-5s %-5s %-5s %-5s %-5s %-5s %-5s\n",
           "Index", "Number", "Fib", "Prim", "Kvad",
           "Cube", "Perf", "Abun", "Def", "Odd");
    printf("----------------------------------------------------------\n");

    /* read file */
    while (fscanf(pfInput, "%d", &iNumber) == 1) {

        pEntry = (struct TASK2_NUMBERS_METADATA*)malloc(
                  sizeof(struct TASK2_NUMBERS_METADATA));
        if (pEntry == NULL) {
            printf("- ERROR: malloc failed\n");
            fclose(pfInput);  pfInput  = NULL;
            fclose(pfOutput); pfOutput = NULL;
            return 1;
        }
        memset(pEntry, 0, sizeof(struct TASK2_NUMBERS_METADATA));

        /* fill struct with results from each function */
        pEntry->iIndex             = iIndex;
        pEntry->iNumber            = iNumber;
        pEntry->bIsFibonacci       = isFibonacci(iNumber)       ? 1 : 0;
        pEntry->bIsPrimeNumber     = isPrime(iNumber)           ? 1 : 0;
        pEntry->bIsSquareNumber    = isSquareNumber(iNumber)    ? 1 : 0;
        pEntry->bIsCubeNumber      = isCubeNumber(iNumber)      ? 1 : 0;
        pEntry->bIsPerfectNumber   = isPerfectNumber(iNumber)   ? 1 : 0;
        pEntry->bIsAbundantNumber  = isAbundantNumber(iNumber)  ? 1 : 0;
        pEntry->bIsDeficientNumber = isDeficientNumber(iNumber) ? 1 : 0;
        pEntry->bIsOddNumber       = isOdd(iNumber)             ? 1 : 0;

        printf("%-5d %-8d %-5d %-5d %-5d %-5d %-5d %-5d %-5d %-5d\n",
               pEntry->iIndex,             pEntry->iNumber,
               pEntry->bIsFibonacci,       pEntry->bIsPrimeNumber,
               pEntry->bIsSquareNumber,    pEntry->bIsCubeNumber,
               pEntry->bIsPerfectNumber,   pEntry->bIsAbundantNumber,
               pEntry->bIsDeficientNumber, pEntry->bIsOddNumber);

        /* write struct as binary to output file */
        fwrite(pEntry, sizeof(struct TASK2_NUMBERS_METADATA), 1, pfOutput);

        free(pEntry);
        pEntry = NULL;
        iIndex++;
    }

    printf("\n- Processed %d numbers.\n", iIndex);

    /* clean up */
    fclose(pfInput);  pfInput  = NULL;
    fclose(pfOutput); pfOutput = NULL;

    puts("*** Program finished. Bye! ***\n");
    return 0;
}
