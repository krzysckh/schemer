#include <stdio.h>

#include "schemer.h"

int main (int argc, char *argv[]) {
  init_gui();
  init_scheme();

  run_gui();

  end_gui();
  end_scheme();
}
