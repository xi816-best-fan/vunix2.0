#include <stdio.h>
#include "panic.h"

void panic(const char* s) {
  printf("Kernel panic - not syncing: %s\n", s);
  for(;;) {}
}
