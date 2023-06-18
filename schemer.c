#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "schemer.h"

static char* argv0;

static void help(void) {
  /* TODO: weźże zrób to co w readme obiecane */
  printf("usage: %s [-h] [init project-name] <file.scm>\n", argv0);

  exit(0);
}

static char *args_shift(int *argc, char ***argv) {
    char *r;

    AOR(**argv) help();

    r = **argv;
    (*argc) -= 1;
    (*argv) += 1;

    return r;
}

static void schemer_init(char *projname) {
  char *buf        = malloc(strlen(projname) + 32),
       dirs[][8]   = { "scm", "res", "build" },
       files[][32] = { "make.scm", "scm/init.scm" };

  int n_dirs  = sizeof(dirs) / sizeof(*dirs),
      n_files = sizeof(files) / sizeof(*files),
      i;

  AOR(mkdir(projname, 0700) != -1)
    err(errno, "fatal error");
  else
    printf("+ %s/\n", projname);

  for (i = 0; i < n_dirs; ++i) {
    snprintf(buf, strlen(projname) + 32, "%s/%s", projname, dirs[i]);

    AOR(mkdir(buf, 0700) != -1)
      err(errno, "fatal error");
    else
      printf("+ %s/\n", buf);
  }

  for (i = 0; i < n_files; ++i) {
    snprintf(buf, strlen(projname) + 32, "%s/%s", projname, files[i]);

    AOR(creat(buf, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH) != -1)
      err(errno, "fatal error");
    else
      printf("+ %s\n", buf);
  }

  free(buf);
  exit(0);
}

int main (int argc, char *argv[]) {
  char *path = NULL,
       *arg;
  extern char *argv0;
  int opt;

  argv0 = args_shift(&argc, &argv);

  while (*argv) {
    arg = args_shift(&argc, &argv);

    S(arg, "-h")
      help();
    else S(arg, "init")
      schemer_init(args_shift(&argc, &argv)); /* exits */
    else
      path = arg;
  }

  /* run */
  init_scheme(path);
  init_gui();

  run_gui();

  end_gui();
  end_scheme();
}
