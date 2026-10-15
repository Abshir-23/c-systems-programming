/* Utlevert startfil, ikke skrevet av meg. */
#include <stdbool.h>
#include <math.h>
bool isSquareNumber(int n) {
   int sqrt_n = (int)sqrt(n);
   return (sqrt_n * sqrt_n == n);
}
