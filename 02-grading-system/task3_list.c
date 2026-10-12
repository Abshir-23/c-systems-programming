
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task3.h"

/* Recalculates the total score by adding up points from all tasks */
void recalc(NODE *pN)
{
    int iIdx = 0;

    pN->iTotalScore = 0;
    for (iIdx = 0; iIdx < NUM_TASKS; iIdx++)
        pN->iTotalScore += pN->aiPoints[iIdx];
}

/* Allocates a new node, zeroes it out and sets the candidate ID */
NODE *create_node(const char *szId)
{
    NODE *pNew = NULL;

    pNew = (NODE *)malloc(sizeof(NODE));
    if (pNew == NULL) {
        printf("ERROR: malloc failed\n");
        return NULL;
    }
    memset(pNew, 0, sizeof(NODE));
    strncpy(pNew->szId, szId, MAX_ID - 1);
    pNew->pPrev = NULL;
    pNew->pNext = NULL;
    return pNew;
}

/* Adds a new candidate to the end of the list */
NODE *add_node(NODE **ppHead, const char *szId)
{
    NODE *pNew    = NULL;
    NODE *pCursor = NULL;

    pNew = create_node(szId);
    if (pNew == NULL) return NULL;

    /* if the list is empty, the new node becomes the head */
    if (*ppHead == NULL) {
        *ppHead = pNew;
        return pNew;
    }

    /* walk to the last node and attach the new one after it */
    pCursor = *ppHead;
    while (pCursor->pNext != NULL)
        pCursor = pCursor->pNext;

    pCursor->pNext = pNew;
    pNew->pPrev    = pCursor;
    return pNew;
}

/* Searches the list for a candidate by ID, returns NULL if not found */
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

/* Removes and frees a single node, re-links its neighbours */
void delete_node(NODE **ppHead, const char *szId)
{
    NODE *pTarget = NULL;

    pTarget = find_node(*ppHead, szId);
    if (pTarget == NULL) {
        printf("Candidate not found\n");
        return;
    }

    /* connect the previous node to the next, skipping the target */
    if (pTarget->pPrev != NULL)
        pTarget->pPrev->pNext = pTarget->pNext;
    else
        *ppHead = pTarget->pNext;   /* target was the head node */

    if (pTarget->pNext != NULL)
        pTarget->pNext->pPrev = pTarget->pPrev;

    free(pTarget);
    pTarget = NULL;
    printf("Candidate deleted\n");
}

/* Walks the entire list and frees every node one by one */
void free_list(NODE **ppHead)
{
    NODE *pCursor = *ppHead;
    NODE *pNext   = NULL;

    while (pCursor != NULL) {
        pNext = pCursor->pNext;  /* save next pointer before freeing */
        free(pCursor);
        pCursor = pNext;
    }
    *ppHead = NULL;
}
