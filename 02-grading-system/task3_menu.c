/* 
   All menu functions for adding, grading, printing and managing candidates.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task3.h"

/* Option 1 - Add a single candidate manually */
void fn_add(NODE **ppHead)
{
    char szId[MAX_ID];

    printf("Candidate ID: ");
    if (!read_line(szId, sizeof(szId))) return;

    if (find_node(*ppHead, szId)) {
        printf("Already exists\n");
        pause_menu();
        return;
    }
    if (add_node(ppHead, szId))
        printf("Added: %s\n", szId);

    pause_menu();
}

/* Option 2 - Load candidates from a text file, one ID per line */
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
        printf("ERROR: Cannot open file: %s\n", szFile);
        return;
    }

    while (fgets(szLine, sizeof(szLine), pFile)) {
        /* strip trailing whitespace and newlines */
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
    printf("Done loading candidates\n");
    pause_menu();
}

/* Option 3 - Grade one specific task for one candidate */
void fn_assess(NODE *pHead)
{
    NODE *pN    = NULL;
    char  szId[MAX_ID];
    char  szBuf[16];
    char  szJust[MAX_JUST];
    int   iTask = 0;
    int   iPts  = 0;

    printf("Candidate ID: ");
    if (!read_line(szId, sizeof(szId))) return;

    pN = find_node(pHead, szId);
    if (pN == NULL) {
        printf("Candidate not found\n");
        return;
    }

    printf("Task (1-%d): ", NUM_TASKS);
    if (!read_line(szBuf, sizeof(szBuf))) return;
    iTask = atoi(szBuf);
    if (iTask < 1 || iTask > NUM_TASKS) {
        printf("Invalid task number\n");
        return;
    }

    printf("Points (0-%d): ", aiTaskMax[iTask - 1]);
    if (!read_line(szBuf, sizeof(szBuf))) return;
    iPts = atoi(szBuf);
    if (iPts < 0 || iPts > aiTaskMax[iTask - 1]) {
        printf("Invalid points\n");
        return;
    }

    printf("Justification: ");
    if (!read_line(szJust, sizeof(szJust))) return;

    pN->aiPoints[iTask - 1] = iPts;
    pN->aiGraded[iTask - 1] = 1;
    strncpy(pN->aszJust[iTask - 1], szJust, MAX_JUST - 1);
    recalc(pN);

    printf("Saved. Total: %d (%s)\n",
           pN->iTotalScore, get_grade(pN->iTotalScore));
    pause_menu();
}

/* Option 4 - Print all candidates with their score and grade */
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

/* Option 5 - Print only candidates with a specific grade */
void fn_by_grade(NODE *pHead)
{
    NODE *pCursor = pHead;
    char  szGrade[4];

    printf("Grade (A-F): ");
    if (!read_line(szGrade, sizeof(szGrade))) return;

    /* convert to uppercase if the user typed lowercase */
    if (szGrade[0] >= 'a' && szGrade[0] <= 'z')
        szGrade[0] = (char)(szGrade[0] - 32);

    while (pCursor != NULL) {
        if (strcmp(get_grade(pCursor->iTotalScore), szGrade) == 0)
            printf("%-12s  %d\n", pCursor->szId, pCursor->iTotalScore);
        pCursor = pCursor->pNext;
    }
    pause_menu();
}

/* Option 6 - Print candidates that still have ungraded tasks */
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

/* Option 7 - Print full details and justifications for one candidate */
void fn_details(NODE *pHead)
{
    NODE *pN   = NULL;
    char  szId[MAX_ID];
    int   iIdx = 0;

    printf("Candidate ID: ");
    if (!read_line(szId, sizeof(szId))) return;

    pN = find_node(pHead, szId);
    if (pN == NULL) {
        printf("Candidate not found\n");
        return;
    }

    printf("\n%s  score: %d  grade: %s\n",
           pN->szId, pN->iTotalScore, get_grade(pN->iTotalScore));

    for (iIdx = 0; iIdx < NUM_TASKS; iIdx++) {
        printf("  Task %d: %d/%d  %s\n",
               iIdx + 1,
               pN->aiPoints[iIdx],
               aiTaskMax[iIdx],
               pN->aiGraded[iIdx] ? pN->aszJust[iIdx] : "(not graded)");
    }
    pause_menu();
}

/* Option 8 - Delete a candidate from the list */
void fn_delete(NODE **ppHead)
{
    char szId[MAX_ID];

    printf("Candidate ID to delete: ");
    if (!read_line(szId, sizeof(szId))) return;

    delete_node(ppHead, szId);
    pause_menu();
}

/* Option 9 - Show statistics: count, average, highest and lowest score */
void fn_stats(NODE *pHead)
{
    NODE *pCursor  = pHead;
    int   iCount   = 0;
    int   iTotal   = 0;
    int   iHighest = 0;
    int   iLowest  = 100;  /* max possible score = sum of aiTaskMax (5+15+20+20+20+20) */

    if (pHead == NULL) {
        printf("No candidates in list\n");
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

    printf("\n--- Statistics ---\n");
    printf("Candidates : %d\n",   iCount);
    printf("Average    : %.1f\n", (double)iTotal / iCount);
    printf("Highest    : %d (%s)\n", iHighest, get_grade(iHighest));
    printf("Lowest     : %d (%s)\n", iLowest,  get_grade(iLowest));
    pause_menu();
}

/* Option 10 - Save all candidates to a binary file */
void fn_save_binary(NODE *pHead)
{
    FILE *pFile   = NULL;
    NODE *pCursor = pHead;
    char  szFile[128];

    printf("Output filename: ");
    if (!read_line(szFile, sizeof(szFile))) return;

    pFile = fopen(szFile, "wb");
    if (pFile == NULL) {
        printf("ERROR: Cannot open file: %s\n", szFile);
        return;
    }

    /* write each node's data to the file, skip the pointer fields */
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
    printf("Saved to %s\n", szFile);
    pause_menu();
}

/* Option 11 - Load candidates from a binary file into the list */
void fn_load_binary(NODE **ppHead)
{
    FILE *pFile  = NULL;
    NODE  sTmp;  
    NODE *pNew   = NULL;
    char  szFile[128];
    int   iRead  = 0;

    printf("Input filename: ");
    if (!read_line(szFile, sizeof(szFile))) return;

    pFile = fopen(szFile, "rb");
    if (pFile == NULL) {
        printf("ERROR: Cannot open file: %s\n", szFile);
        return;
    }

    memset(&sTmp, 0, sizeof(NODE));

    /* read one candidate at a time until we reach end of file */
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
    printf("Done loading from binary\n");
    pause_menu();
}
