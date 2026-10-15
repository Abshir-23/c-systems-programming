/* 
   Alle menyfunksjonene for å legge til, vurdere, skrive ut og administrere kandidater.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task3.h"

/* Valg 1 - Legg til én kandidat manuelt */
void fn_add(NODE **ppHead)
{
    char szId[MAX_ID];

    printf("Kandidat-ID: ");
    if (!read_line(szId, sizeof(szId))) return;

    if (find_node(*ppHead, szId)) {
        printf("Finnes allerede\n");
        pause_menu();
        return;
    }
    if (add_node(ppHead, szId))
        printf("Added: %s\n", szId);

    pause_menu();
}

/* Valg 2 - Last inn kandidater fra en tekstfil, én ID per linje */
void fn_load_text(NODE **ppHead)
{
    FILE *pFile  = NULL;
    char  szFile[128];
    char  szLine[256];
    char *pEnd   = NULL;

    printf("Filename: ");
    if (!read_line(szFile, sizeof(szFile))) return;

    pFile = fopen(szFile, "r");
    if (pFile == NULL) {
        printf("FEIL: Kan ikke åpne filen: %s\n", szFile);
        return;
    }

    while (fgets(szLine, sizeof(szLine), pFile)) {
        /* fjern mellomrom og linjeskift på slutten */
        pEnd = szLine + strlen(szLine) - 1;
        while (pEnd >= szLine &&
               (*pEnd == '\n' || *pEnd == '\r' || *pEnd == ' '))
            *pEnd-- = '\0';

        if (strlen(szLine) == 0) continue;

        if (!find_node(*ppHead, szLine))
            add_node(ppHead, szLine);
    }

    fclose(pFile);
    pFile = NULL;
    printf("Ferdig med å laste inn kandidater\n");
    pause_menu();
}

/* Valg 3 - Vurder én bestemt oppgave for én kandidat */
void fn_assess(NODE *pHead)
{
    NODE *pN    = NULL;
    char  szId[MAX_ID];
    char  szBuf[16];
    char  szJust[MAX_JUST];
    int   iTask = 0;
    int   iPts  = 0;

    printf("Kandidat-ID: ");
    if (!read_line(szId, sizeof(szId))) return;

    pN = find_node(pHead, szId);
    if (pN == NULL) {
        printf("Fant ikke kandidaten\n");
        return;
    }

    printf("Task (1-%d): ", NUM_TASKS);
    if (!read_line(szBuf, sizeof(szBuf))) return;
    iTask = atoi(szBuf);
    if (iTask < 1 || iTask > NUM_TASKS) {
        printf("Ugyldig oppgavenummer\n");
        return;
    }

    printf("Points (0-%d): ", aiTaskMax[iTask - 1]);
    if (!read_line(szBuf, sizeof(szBuf))) return;
    iPts = atoi(szBuf);
    if (iPts < 0 || iPts > aiTaskMax[iTask - 1]) {
        printf("Ugyldig poengsum\n");
        return;
    }

    printf("Justification: ");
    if (!read_line(szJust, sizeof(szJust))) return;

    pN->aiPoints[iTask - 1] = iPts;
    pN->aiGraded[iTask - 1] = 1;
    strncpy(pN->aszJust[iTask - 1], szJust, MAX_JUST - 1);
    recalc(pN);

    printf("Lagret. Totalt: %d (%s)\n",
           pN->iTotalScore, get_grade(pN->iTotalScore));
    pause_menu();
}

/* Valg 4 - Skriv ut alle kandidater med poengsum og karakter */
void fn_print_all(NODE *pHead)
{
    NODE *pCursor = pHead;

    printf("\n%-12s  %6s  %5s\n", "Candidate", "Score", "Grade");
    printf("%-12s  %6s  %5s\n",   "------------", "------", "-----");

    while (pCursor != NULL) {
        printf("%-12s  %6d  %5s\n",
               pCursor->szId,
               pCursor->iTotalScore,
               get_grade(pCursor->iTotalScore));
        pCursor = pCursor->pNext;
    }
    pause_menu();
}

/* Valg 5 - Skriv bare ut kandidater med en bestemt karakter */
void fn_by_grade(NODE *pHead)
{
    NODE *pCursor = pHead;
    char  szGrade[4];

    printf("Grade (A-F): ");
    if (!read_line(szGrade, sizeof(szGrade))) return;

    /* gjør om til stor bokstav hvis brukeren skrev liten */
    if (szGrade[0] >= 'a' && szGrade[0] <= 'z')
        szGrade[0] = (char)(szGrade[0] - 32);

    while (pCursor != NULL) {
        if (strcmp(get_grade(pCursor->iTotalScore), szGrade) == 0)
            printf("%-12s  %d\n", pCursor->szId, pCursor->iTotalScore);
        pCursor = pCursor->pNext;
    }
    pause_menu();
}

/* Valg 6 - Skriv ut kandidater som fortsatt har oppgaver som ikke er vurdert */
void fn_ungraded(NODE *pHead)
{
    NODE *pCursor = pHead;
    int   iIdx    = 0;

    while (pCursor != NULL) {
        for (iIdx = 0; iIdx < NUM_TASKS; iIdx++) {
            if (!pCursor->aiGraded[iIdx])
                printf("%s - task %d not graded\n",
                       pCursor->szId, iIdx + 1);
        }
        pCursor = pCursor->pNext;
    }
    pause_menu();
}

/* Valg 7 - Skriv ut alle detaljer og begrunnelser for én kandidat */
void fn_details(NODE *pHead)
{
    NODE *pN   = NULL;
    char  szId[MAX_ID];
    int   iIdx = 0;

    printf("Kandidat-ID: ");
    if (!read_line(szId, sizeof(szId))) return;

    pN = find_node(pHead, szId);
    if (pN == NULL) {
        printf("Fant ikke kandidaten\n");
        return;
    }

    printf("\n%s  poeng: %d  karakter: %s\n",
           pN->szId, pN->iTotalScore, get_grade(pN->iTotalScore));

    for (iIdx = 0; iIdx < NUM_TASKS; iIdx++) {
        printf("  Oppgave %d: %d/%d  %s\n",
               iIdx + 1,
               pN->aiPoints[iIdx],
               aiTaskMax[iIdx],
               pN->aiGraded[iIdx] ? pN->aszJust[iIdx] : "(ikke vurdert)");
    }
    pause_menu();
}

/* Valg 8 - Slett en kandidat fra listen */
void fn_delete(NODE **ppHead)
{
    char szId[MAX_ID];

    printf("Kandidat-ID som skal slettes: ");
    if (!read_line(szId, sizeof(szId))) return;

    delete_node(ppHead, szId);
    pause_menu();
}

/* Valg 9 - Vis statistikk: antall, snitt, høyeste og laveste poengsum */
void fn_stats(NODE *pHead)
{
    NODE *pCursor  = pHead;
    int   iCount   = 0;
    int   iTotal   = 0;
    int   iHighest = 0;
    int   iLowest  = 100;  /* høyest mulige poengsum = summen av aiTaskMax (5+15+20+20+20+20) */

    if (pHead == NULL) {
        printf("Ingen kandidater i listen\n");
        pause_menu();
        return;
    }

    while (pCursor != NULL) {
        iCount++;
        iTotal += pCursor->iTotalScore;
        if (pCursor->iTotalScore > iHighest)
            iHighest = pCursor->iTotalScore;
        if (pCursor->iTotalScore < iLowest)
            iLowest = pCursor->iTotalScore;
        pCursor = pCursor->pNext;
    }

    printf("\n--- Statistikk ---\n");
    printf("Candidates : %d\n",   iCount);
    printf("Snitt      : %.1f\n", (double)iTotal / iCount);
    printf("Høyeste    : %d (%s)\n", iHighest, get_grade(iHighest));
    printf("Laveste    : %d (%s)\n", iLowest,  get_grade(iLowest));
    pause_menu();
}

/* Valg 10 - Lagre alle kandidater i en binærfil */
void fn_save_binary(NODE *pHead)
{
    FILE *pFile   = NULL;
    NODE *pCursor = pHead;
    char  szFile[128];

    printf("Navn på utdatafil: ");
    if (!read_line(szFile, sizeof(szFile))) return;

    pFile = fopen(szFile, "wb");
    if (pFile == NULL) {
        printf("FEIL: Kan ikke åpne filen: %s\n", szFile);
        return;
    }

    /* skriv dataene i hver node til filen, og hopp over pekerfeltene */
    while (pCursor != NULL) {
        fwrite(pCursor->szId,         sizeof(pCursor->szId),     1, pFile);
        fwrite(pCursor->aiPoints,     sizeof(pCursor->aiPoints), 1, pFile);
        fwrite(pCursor->aszJust,      sizeof(pCursor->aszJust),  1, pFile);
        fwrite(pCursor->aiGraded,     sizeof(pCursor->aiGraded), 1, pFile);
        fwrite(&pCursor->iTotalScore, sizeof(int),               1, pFile);
        pCursor = pCursor->pNext;
    }

    fclose(pFile);
    pFile = NULL;
    printf("Lagret i %s\n", szFile);
    pause_menu();
}

/* Valg 11 - Last inn kandidater fra en binærfil til listen */
void fn_load_binary(NODE **ppHead)
{
    FILE *pFile  = NULL;
    NODE  sTmp;  
    NODE *pNew   = NULL;
    char  szFile[128];
    int   iRead  = 0;

    printf("Navn på inndatafil: ");
    if (!read_line(szFile, sizeof(szFile))) return;

    pFile = fopen(szFile, "rb");
    if (pFile == NULL) {
        printf("FEIL: Kan ikke åpne filen: %s\n", szFile);
        return;
    }

    memset(&sTmp, 0, sizeof(NODE));

    /* les én kandidat om gangen til vi når slutten av filen */
    while (1) {
        iRead = (int)fread(sTmp.szId, sizeof(sTmp.szId), 1, pFile);
        if (iRead != 1) break;

        fread(sTmp.aiPoints,      sizeof(sTmp.aiPoints), 1, pFile);
        fread(sTmp.aszJust,       sizeof(sTmp.aszJust),  1, pFile);
        fread(sTmp.aiGraded,      sizeof(sTmp.aiGraded), 1, pFile);
        fread(&sTmp.iTotalScore,  sizeof(int),           1, pFile);

        if (!find_node(*ppHead, sTmp.szId)) {
            pNew = add_node(ppHead, sTmp.szId);
            if (pNew != NULL) {
                memcpy(pNew->aiPoints, sTmp.aiPoints, sizeof(sTmp.aiPoints));
                memcpy(pNew->aszJust,  sTmp.aszJust,  sizeof(sTmp.aszJust));
                memcpy(pNew->aiGraded, sTmp.aiGraded, sizeof(sTmp.aiGraded));
                pNew->iTotalScore = sTmp.iTotalScore;
            }
        }
    }

    fclose(pFile);
    pFile = NULL;
    printf("Ferdig med å laste inn fra binærfil\n");
    pause_menu();
}
