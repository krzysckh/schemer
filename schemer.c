#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "schemer.h"

static char *argv0;
static void help() {
  /* TODO: weźże zrób to co w readme obiecane */
  printf("usage: %s [-h] file\n", argv0);

  exit(0);
}

int main (int argc, char *argv[]) {
  char *path = NULL;
  int opt;

  argv0 = *argv;

  while ((opt = getopt(argc, argv, "h")) != -1) {
    switch (opt) {
      default:
        help();
    }
  }

  path = argv[optind];

  /* run */
  init_gui();
  init_scheme(path);

  run_gui();

  end_gui();
  end_scheme();
}
