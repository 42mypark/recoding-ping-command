#include <stdio.h>
#include <stdlib.h>

void fatal_error_check(int condition, char *msg) {
  if (condition) {
    fprintf(stderr, "Fatal Error: %s\n", msg);
    exit(1);
  }
}