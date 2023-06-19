#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>

#include "schemer.h"

char** scheme_args = NULL;
char* argv0        = NULL;
int n_scheme_args  = 0;

static void help(void) {
  /* TODO: weźże zrób to co w readme obiecane */
  printf("usage: %s [-h] [init project-name] <file.scm> [--] [script args]\n",
      argv0);

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

static void schemer_run(void) {
  char init_path[] = "./scm/init.scm",
       make_path[] = "./make.scm";

  init_scheme(make_path);
  scm_ctx_add(init_path);

  init_gui();

  run_gui();

  end_gui();
  end_scheme();

  exit(0);
}

static void schemer_build(void) {
  errx(1, "build not implemented");
}

static void add_scheme_arg(char *s) {
  scheme_args = realloc(scheme_args, sizeof(char*) * (n_scheme_args + 1));
  scheme_args[n_scheme_args++] = s; /* don't copy strings, just addresses */
}

int main (int argc, char *argv[]) {
  char *path = NULL,
       *arg;
  extern char *argv0;
  int skip = 0;

  argv0 = args_shift(&argc, &argv);

  while (*argv) {
    arg = args_shift(&argc, &argv);

    S(arg, "-h")
      if (!skip) help();
      else       add_scheme_arg(arg);
    else S(arg, "init")
      if (!skip) schemer_init(args_shift(&argc, &argv)); /* exits */
      else       add_scheme_arg(arg);
    else S(arg, "run")
      if (!skip) schemer_run(); /* exits */
      else       add_scheme_arg(arg);
    else S(arg, "build")
      if (!skip) schemer_build(); /* exits */
      else       add_scheme_arg(arg);
    else S(arg, "--")
      if (!skip) skip = 1;
      else       add_scheme_arg(arg);
    else if (!path)
      if (!skip) path = arg;
      else       add_scheme_arg(arg);
    else
      add_scheme_arg(arg);
  }

  /* run */
  init_scheme(path);
  init_gui();

  run_gui();

  end_gui();
  end_scheme();
}
