#include "schemer.h"

#include <chibi/eval.h>

#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <err.h>
#include <libgen.h>

#ifdef __linux__
#include <linux/limits.h>
#else
#include <sys/syslimits.h>
#endif

static void strreplace(char *s, char a, char b) {
  size_t sz = strlen(s),
         i;

  for (i = 0; i < sz; ++i)
    if (s[i] == a)
      s[i] = b;
}

static size_t get_file_size(FILE *f) {
  size_t ret;

  A(f);

  fseek(f, 0, SEEK_END);
  ret = ftell(f);
  fseek(f, 0, SEEK_SET);

  return ret;
}

void filename_to_cfun(char *path) {
  strreplace(path, '-', '_');
  strreplace(path, '/', '_');
  strreplace(path, '.', '_');
}

void any_to_c(char *path, char *opath) {
  FILE *f, *of;
  char out_filename[PATH_MAX],
       *cfun;
  int sz = 0, c;

  cfun = strdup(path);
  filename_to_cfun(cfun);

  f = fopen(path, "r");
  AOR(f) errx(1, "cannot open %s", path);
  sz = get_file_size(f);

  if (opath != NULL)
    snprintf(out_filename, PATH_MAX, "%s", opath);
  else
    snprintf(out_filename, PATH_MAX, "%s.c", path);

  of = fopen(out_filename, "w");
  AOR(of) errx(1, "cannot open %s", out_filename);

  fprintf(of, "static char v[] = {\n");
  while (!feof(f)) {
    c = fgetc(f);
    if (feof(f))
      break;
    fprintf(of, "  %d,\n", c);
  }
  fprintf(of, "  0\n};");

  fprintf(of, "\n\n\
static long int v_l = %d;\n\
\n\
#include <chibi/eval.h>\n\
#include <stdio.h>\n\
#include <stdlib.h>\n\
#include <string.h>\n\
\n\
int get_length_of_%s(void) { return v_l; }\n\
char *get_contents_of_%s(void) {\n\
  char *ret = malloc(v_l + 1); int i;\n\
  for (i = 0; i < v_l; ++i) ret[i] = v[i]; return ret;}\n\
int include_%s(sexp ctx) {\n\
  sexp e, obj;\n\
  char *fname = tmpnam(NULL);\n\
  FILE *f = fopen(fname, \"w\");\n\
\n\
  fwrite(v, 1, v_l, f);\n\
  fclose(f);\n\
\n\
  obj = sexp_c_string(ctx, fname, -1);\n\
  e = sexp_load(ctx, obj, NULL);\n\
\n\
  return print_if_exception(e);\n\
}\n", sz, cfun, cfun, cfun);


  printf("-> %s\n", out_filename);
  free(cfun);
  fclose(of);
  fclose(f);
}

#ifndef ANY2C
extern unsigned char *__third_party_chibi_scheme_libchibi_scheme_a;
extern unsigned int __third_party_chibi_scheme_libchibi_scheme_a_len;

static char **compiler_resource_paths = NULL;
static int n_compiler_resource_paths = 0;

void schemer_add_resource(char *path) {
  compiler_resource_paths = realloc(compiler_resource_paths,
      sizeof(char *) * (n_compiler_resource_paths + 1));

  compiler_resource_paths[n_compiler_resource_paths] = strdup(path);
  n_compiler_resource_paths++;
}

void schemer_compile(void) {
  extern sexp scm_ctx;
  DIR *build_dir;
  char opath[PATH_MAX];
  int i;

  init_scheme("make.scm");

  build_dir = opendir("./build");
  AOR(build_dir)
    errx(1, "build directory doesn't exist. are you sure you are in a "
        "schemer project?");
  closedir(build_dir);

  AOR(mkdir("./build/resources", 0700) != -1
      || access("./build/resources", F_OK) != -1)
    errx(1, "cannot create ./build/resources");

  for (i = 0; i < n_compiler_resource_paths; ++i) {
    snprintf(opath, PATH_MAX, "./build/resources/%s.c",
        basename(compiler_resource_paths[i]));
    any_to_c(compiler_resource_paths[i], opath);
  }

  print_if_exception(
      sexp_eval_string(scm_ctx, "(__make-continue__)", -1, NULL));

  end_scheme();
}

void schemer_free_resources(void) {
  int i;

  for (i = 0; i < n_compiler_resource_paths; ++i)
    free(compiler_resource_paths[i]);

  free(compiler_resource_paths);
}

#endif /* ANY2C */
