#include <stdio.h>

void fatal_error_check(int condition) {
  if (condition) {
    fprintf(stderr, "Fatal Error\n");
    exit(1);
  }
}