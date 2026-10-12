/* 
   Entry point for the program. Contains the menu, grade calculation
   and small utility functions used throughout the code.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task3.h"

/* Maximum points per task  */
int aiTaskMax[NUM_TASKS] = {5, 15, 20, 20, 20, 20};

/* Waits for the user to press ENTER before returning to the menu */
void pause_menu(void)
{
    printf("\n--- Press ENTER to continue ---");
    while (getchar() != '\n');
}

/* Safely reads a line from keyboard using fgets, strips newline */
int read_line(char *szBuf, int iSize)
{
    if (fgets(szBuf, iSize, stdin) == NULL) return 0;
    szBuf[strcspn(szBuf, "\r\n")] = '\0';
    return 1;
}

/* Converts total score to letter grade using the A to F grading scale */
const char *get_grade(int iScore)
{
    if (iScore >= 89) return "A";
    if (iScore >= 77) return "B";
    if (iScore >= 65) return "C";
    if (iScore >= 53) return "D";
    if (iScore >= 41) return "E";
    return "F";
}

/* Prints the main menu to the screen */
static void print_menu(void)
{
    printf("\n+------------------------------+\n");
    printf("|     Grading System      |\n");
    printf("+------------------------------+\n");
    printf("|  1. Add candidate            |\n");
    printf("|  2. Load from text file      |\n");
    printf("|  3. Grade a task             |\n");
    printf("|  4. Print all candidates     |\n");
    printf("|  5. Print by grade           |\n");
    printf("|  6. Print ungraded           |\n");
    printf("|  7. Print details            |\n");
    printf("|  8. Delete candidate         |\n");
    printf("|  9. Statistics               |\n");
    printf("| 10. Save to binary file      |\n");
    printf("| 11. Load from binary file    |\n");
    printf("|  0. Exit                     |\n");
    printf("+------------------------------+\n");
    printf("Choice: ");
}

/* Reads the user's menu choice and returns it as an enum value */
static enum MENU get_choice(void)
{
    char szBuf[8];
    if (!read_line(szBuf, sizeof(szBuf))) return MENU_EXIT;
    return (enum MENU)atoi(szBuf);
}

int main(void)
{
    NODE      *pHead   = NULL;
    enum MENU  eChoice = MENU_EXIT;

    do {
        print_menu();
        eChoice = get_choice();

        switch (eChoice) {
            case MENU_ADD:      fn_add(&pHead);         break;
            case MENU_LOAD:     fn_load_text(&pHead);   break;
            case MENU_ASSESS:   fn_assess(pHead);       break;
            case MENU_ALL:      fn_print_all(pHead);    break;
            case MENU_BYGRADE:  fn_by_grade(pHead);     break;
            case MENU_UNGRADED: fn_ungraded(pHead);     break;
            case MENU_DETAILS:  fn_details(pHead);      break;
            case MENU_DELETE:   fn_delete(&pHead);      break;
            case MENU_STATS:    fn_stats(pHead);        break;
            case MENU_SAVE_BIN: fn_save_binary(pHead);  break;
            case MENU_LOAD_BIN: fn_load_binary(&pHead); break;
            case MENU_EXIT:
                free_list(&pHead);  /* free all memory before exit */
                printf("Goodbye!\n");
                break;
            default:
                printf("Invalid choice\n");
                break;
        }
    } while (eChoice != MENU_EXIT);

    return 0;
}
