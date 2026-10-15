/* 

   Dette programmet leser tall fra en tekstfil, kjører hvert tall
   gjennom 8 mattefunksjoner og lagrer resultatene i en binærfil.
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

    /* sjekk argumentene */
    if (iArgc != 2) {
        printf("FEIL: Feil parametere - Bruk følgende ved oppstart:\n");
        printf("        $ ./task2 <filnavn>\n");
        printf("Prøv igjen. Avslutter programmet...\n");
        return 1;
    }

    puts("\n*** Tallklassifisering ***\n");

    /* åpne inndatafilen */
    pfInput = fopen(apszArgv[1], "r");
    if (pfInput == NULL) {
        printf("- FEIL: Kunne ikke åpne inndatafilen: %s\n", apszArgv[1]);
        return 1;
    }
    printf("- Åpnet inndatafilen: \"%s\"\n", apszArgv[1]);

    /* åpne utdatafilen for binær skriving */
    pfOutput = fopen("output.bin", "wb");
    if (pfOutput == NULL) {
        printf("- FEIL: Kunne ikke åpne utdatafilen\n");
        fclose(pfInput);
        pfInput = NULL;
        return 1;
    }
    printf("- Åpnet utdatafilen: \"output.bin\"\n\n");

    /* skriv ut tabelloverskriften */
    printf("%-5s %-8s %-5s %-5s %-5s %-5s %-5s %-5s %-5s %-5s\n",
           "Index", "Number", "Fib", "Prim", "Kvad",
           "Cube", "Perf", "Abun", "Def", "Odd");
    printf("----------------------------------------------------------\n");

    /* les filen */
    while (fscanf(pfInput, "%d", &iNumber) == 1) {

        pEntry = (struct TASK2_NUMBERS_METADATA*)malloc(
                  sizeof(struct TASK2_NUMBERS_METADATA));
        if (pEntry == NULL) {
            printf("- FEIL: malloc feilet\n");
            fclose(pfInput);  pfInput  = NULL;
            fclose(pfOutput); pfOutput = NULL;
            return 1;
        }
        memset(pEntry, 0, sizeof(struct TASK2_NUMBERS_METADATA));

        /* fyll structen med resultatene fra hver funksjon */
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

        /* skriv structen binært til utdatafilen */
        fwrite(pEntry, sizeof(struct TASK2_NUMBERS_METADATA), 1, pfOutput);

        free(pEntry);
        pEntry = NULL;
        iIndex++;
    }

    printf("\n- Behandlet %d tall.\n", iIndex);

    /* rydd opp */
    fclose(pfInput);  pfInput  = NULL;
    fclose(pfOutput); pfOutput = NULL;

    puts("*** Programmet er ferdig. Ha det! ***\n");
    return 0;
}
