#include "schemer.h"

#include <chibi/eval.h>

#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <err.h>

#ifdef __linux__
#include <linux/limits.h>
#else
#include <sys/syslimits.h>
#endif

extern unsigned char *__third_party_chibi_scheme_libchibi_scheme_a;
extern unsigned int __third_party_chibi_scheme_libchibi_scheme_a_len;

static char **compiler_resource_paths = NULL;
static int n_compiler_resource_paths = 0;

static size_t get_file_size(FILE *f) {
  size_t ret;

  A(f);

  fseek(f, 0, SEEK_END);
  ret = ftell(f);
  fseek(f, 0, SEEK_SET);

  return ret;
}

static void strreplace(char *s, char a, char b) {
  size_t sz = strlen(s),
         i;

  for (i = 0; i < sz; ++i)
    if (s[i] == a)
      s[i] = b;
}

static void filename_to_cfun(char *path) {
  strreplace(path, '-', '_');
  strreplace(path, '/', '_');
  strreplace(path, '.', '_');
}

/* same as scm2bin.scm */
void any_to_c(char *path) {
  FILE *f, *of;
  char out_filename[PATH_MAX],
       *cfun;
  int sz = 0, c;

  cfun = strdup(path);
  filename_to_cfun(cfun);

  f = fopen(path, "r");
  AOR(f) errx(1, "cannot open %s", path);

  snprintf(out_filename, PATH_MAX, "%s.c", path);

  of = fopen(out_filename, "w");
  AOR(of) errx(1, "cannot open %s", out_filename);

  fprintf(of, "static char v[] = {\n");
  while (!feof(f)) {
    c = fgetc(f);
    if (feof(f))
      break;
    fprintf(of, "  %d,\n", c);
    sz++;
  }
  fprintf(of, "  0\n};");

  fprintf(of, "\n\n\
static long int v_l = %d;\n\
\n\
#include <chibi/eval.h>\n\
#include <stdio.h>\n\
#include \"schemer.h\"\n\
\n\
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
}\n", sz, cfun);


  printf("-> %s\n", out_filename);
  free(cfun);
  fclose(of);
  fclose(f);
}

void schemer_add_resource(char *path) {
  compiler_resource_paths = realloc(compiler_resource_paths,
      sizeof(char *) * (n_compiler_resource_paths + 1));

  compiler_resource_paths[n_compiler_resource_paths] = strdup(path);
}

void schemer_compile(void) {
  DIR *build_dir;

  build_dir = opendir("./build");
  AOR(build_dir)
    errx(1, "build directory doesn't exist. are you sure you are in a "
        "schemer project?");
}

void schemer_free_resources(void) {
  int i;

  for (i = 0; i < n_compiler_resource_paths; ++i)
    free(compiler_resource_paths[i]);

  free(compiler_resource_paths);
}
