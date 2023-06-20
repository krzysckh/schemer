#include <stdio.h>
#include <unistd.h>
#include <err.h>

#include "schemer.h"

int main (int argc, char *argv[]) {
  A(argv[1]);
  A(access(argv[1], F_OK) != -1);

  any_to_c(argv[1]);

  return 0;
}
