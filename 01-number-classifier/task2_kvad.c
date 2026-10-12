/* Provided starter file, not written by me. */
#include <stdbool.h>
#include <math.h>
bool isSquareNumber(int n) {
   int sqrt_n = (int)sqrt(n);
   return (sqrt_n * sqrt_n == n);
}
