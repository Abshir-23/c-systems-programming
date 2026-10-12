/* Provided starter file, not written by me. */
#include <stdbool.h>
bool isAbundantNumber(int n) {
   int i = 0;
   int sum = 0;
   for (i = 1; i < n; i++) {
      if (n % i == 0) {
         sum += i;
      }
   }
   return (sum > n);
}
