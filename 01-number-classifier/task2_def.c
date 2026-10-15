/* Utlevert startfil, ikke skrevet av meg. */
#include <stdbool.h>
bool isDeficientNumber(int n) {
   int i = 0;
   int sum = 0;
   for (i = 1; i < n; i++) {
      if (n % i == 0) {
         sum += i;
      }
   }
   return (sum < n);
}
