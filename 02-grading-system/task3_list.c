
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task3.h"

/* Regner ut total poengsum på nytt ved å legge sammen poengene fra alle oppgavene */
void recalc(NODE *pN)
{
    int iIdx = 0;

    pN->iTotalScore = 0;
    for (iIdx = 0; iIdx < NUM_TASKS; iIdx++)
        pN->iTotalScore += pN->aiPoints[iIdx];
}

/* Allokerer en ny node, nullstiller den og setter kandidat-ID */
NODE *create_node(const char *szId)
{
    NODE *pNew = NULL;

    pNew = (NODE *)malloc(sizeof(NODE));
    if (pNew == NULL) {
        printf("FEIL: malloc feilet\n");
        return NULL;
    }
    memset(pNew, 0, sizeof(NODE));
    strncpy(pNew->szId, szId, MAX_ID - 1);
    pNew->pPrev = NULL;
    pNew->pNext = NULL;
    return pNew;
}

/* Legger til en ny kandidat sist i listen */
NODE *add_node(NODE **ppHead, const char *szId)
{
    NODE *pNew    = NULL;
    NODE *pCursor = NULL;

    pNew = create_node(szId);
    if (pNew == NULL) return NULL;

    /* hvis listen er tom, blir den nye noden hodet */
    if (*ppHead == NULL) {
        *ppHead = pNew;
        return pNew;
    }

    /* gå til siste node og fest den nye etter den */
    pCursor = *ppHead;
    while (pCursor->pNext != NULL)
        pCursor = pCursor->pNext;

    pCursor->pNext = pNew;
    pNew->pPrev    = pCursor;
    return pNew;
}

/* Søker etter en kandidat med ID i listen, returnerer NULL hvis den ikke finnes */
NODE *find_node(NODE *pHead, const char *szId)
{
    NODE *pCursor = pHead;

    while (pCursor != NULL) {
        if (strcmp(pCursor->szId, szId) == 0)
            return pCursor;
        pCursor = pCursor->pNext;
    }
    return NULL;
}

/* Fjerner og frigjør én node, og kobler naboene sammen igjen */
void delete_node(NODE **ppHead, const char *szId)
{
    NODE *pTarget = NULL;

    pTarget = find_node(*ppHead, szId);
    if (pTarget == NULL) {
        printf("Fant ikke kandidaten\n");
        return;
    }

    /* koble forrige node til neste, og hopp over den som skal slettes */
    if (pTarget->pPrev != NULL)
        pTarget->pPrev->pNext = pTarget->pNext;
    else
        *ppHead = pTarget->pNext;   /* noden som skal slettes var hodet */

    if (pTarget->pNext != NULL)
        pTarget->pNext->pPrev = pTarget->pPrev;

    free(pTarget);
    pTarget = NULL;
    printf("Kandidaten er slettet\n");
}

/* Går gjennom hele listen og frigjør hver node en etter en */
void free_list(NODE **ppHead)
{
    NODE *pCursor = *ppHead;
    NODE *pNext   = NULL;

    while (pCursor != NULL) {
        pNext = pCursor->pNext;  /* lagre neste-pekeren før frigjøring */
        free(pCursor);
        pCursor = pNext;
    }
    *ppHead = NULL;
}
