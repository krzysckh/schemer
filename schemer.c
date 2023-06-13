#include <stdio.h>

#include "schemer.h"

int main (int argc, char *argv[]) {
  init_scheme();
  init_gui();

  run_gui();

  end_gui();
  end_scheme();
}
