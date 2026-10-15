/* 
   Startpunktet for programmet. Inneholder menyen, utregning av karakter
   og små hjelpefunksjoner som brukes i hele koden.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task3.h"

/* Maks poeng per oppgave  */
int aiTaskMax[NUM_TASKS] = {5, 15, 20, 20, 20, 20};

/* Venter til brukeren trykker ENTER før den går tilbake til menyen */
void pause_menu(void)
{
    printf("\n--- Trykk ENTER for å fortsette ---");
    while (getchar() != '\n');
}

/* Leser en linje fra tastaturet trygt med fgets, og fjerner linjeskift */
int read_line(char *szBuf, int iSize)
{
    if (fgets(szBuf, iSize, stdin) == NULL) return 0;
    szBuf[strcspn(szBuf, "\r\n")] = '\0';
    return 1;
}

/* Gjør om total poengsum til bokstavkarakter etter karakterskalaen A til F */
const char *get_grade(int iScore)
{
    if (iScore >= 89) return "A";
    if (iScore >= 77) return "B";
    if (iScore >= 65) return "C";
    if (iScore >= 53) return "D";
    if (iScore >= 41) return "E";
    return "F";
}

/* Skriver ut hovedmenyen på skjermen */
static void print_menu(void)
{
    printf("\n+------------------------------+\n");
    printf("|     Karaktersystem      |\n");
    printf("+------------------------------+\n");
    printf("|  1. Legg til kandidat        |\n");
    printf("|  2. Last inn fra tekstfil    |\n");
    printf("|  3. Vurder en oppgave        |\n");
    printf("|  4. Skriv ut alle kandidater |\n");
    printf("|  5. Skriv ut etter karakter  |\n");
    printf("|  6. Skriv ut ikke vurderte   |\n");
    printf("|  7. Skriv ut detaljer        |\n");
    printf("|  8. Slett kandidat           |\n");
    printf("|  9. Statistikk               |\n");
    printf("| 10. Lagre i binærfil         |\n");
    printf("| 11. Last inn fra binærfil    |\n");
    printf("|  0. Avslutt                  |\n");
    printf("+------------------------------+\n");
    printf("Valg: ");
}

/* Leser brukerens menyvalg og returnerer det som en enum-verdi */
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
                free_list(&pHead);  /* frigjør alt minne før avslutning */
                printf("Goodbye!\n");
                break;
            default:
                printf("Ugyldig valg\n");
                break;
        }
    } while (eChoice != MENU_EXIT);

    return 0;
}
