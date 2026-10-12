/* Provided starter file, not written by me. */
#include <stdbool.h>
#include <math.h>
bool isCubeNumber(int n) {
   int cube_n = (int)cbrt(n);
   return (cube_n * cube_n * cube_n == n);
}
