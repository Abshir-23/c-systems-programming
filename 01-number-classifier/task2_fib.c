/* Provided starter file, not written by me. */
#include <stdbool.h>
bool isFibonacci(int n) {
   int a = 0, b = 1, c = a + b;
   while (c < n) {
      a = b;
      b = c;
      c = a + b;
   }
   return (c == n);
}
