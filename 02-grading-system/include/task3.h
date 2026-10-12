#ifndef TASK3_H
#define TASK3_H

/* Max points allowed per task, defined in task3.c */
#define NUM_TASKS  6
#define MAX_ID     16
#define MAX_JUST   256

extern int aiTaskMax[NUM_TASKS];

/* A student in the grading list */
typedef struct node {
    char          szId[MAX_ID];           
    int           aiPoints[NUM_TASKS];    
    char          aszJust[NUM_TASKS][MAX_JUST]; 
    int           aiGraded[NUM_TASKS];    
    int           iTotalScore;            
    struct node  *pPrev;
    struct node  *pNext;
} NODE;

/* Main menu options */
enum MENU {
    MENU_ADD      = 1,
    MENU_LOAD     = 2,
    MENU_ASSESS   = 3,
    MENU_ALL      = 4,
    MENU_BYGRADE  = 5,
    MENU_UNGRADED = 6,
    MENU_DETAILS  = 7,
    MENU_DELETE   = 8,
    MENU_STATS    = 9,
    MENU_SAVE_BIN = 10,
    MENU_LOAD_BIN = 11,
    MENU_EXIT     = 0
};

/* Linked list operations */
NODE       *create_node(const char *szId);
NODE       *add_node(NODE **ppHead, const char *szId);
NODE       *find_node(NODE *pHead, const char *szId);
void        free_list(NODE **ppHead);
void        recalc(NODE *pN);        /* Recalculate total score */
void        delete_node(NODE **ppHead, const char *szId);

/* Menu actions */
void        fn_add(NODE **ppHead);
void        fn_load_text(NODE **ppHead);
void        fn_assess(NODE *pHead);
void        fn_print_all(NODE *pHead);
void        fn_by_grade(NODE *pHead);
void        fn_ungraded(NODE *pHead);
void        fn_details(NODE *pHead);
void        fn_delete(NODE **ppHead);
void        fn_stats(NODE *pHead);
void        fn_save_binary(NODE *pHead);
void        fn_load_binary(NODE **ppHead);

/* Helpers */
const char *get_grade(int iScore);   /* Convert score to letter grade */
int         read_line(char *szBuf, int iSize);
void        pause_menu(void);

#endif
