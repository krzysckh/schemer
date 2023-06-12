#include "schemer.h"

#include <stdio.h>
#include <assert.h>

#include <chibi/eval.h>
#include <chibi/gc_heap.h>

typedef enum {
  GT_ALL,
  GT_MATH,
  GT_PLOT,
  GT_GAME
} Pref_graphics_type;

static sexp scm_ctx = NULL;
static Pref_graphics_type graphics_type = GT_ALL;

static void print_if_exception(sexp s) {
  if (sexp_exceptionp(s))
    sexp_print_exception(scm_ctx, s,
        sexp_make_output_port(scm_ctx, stdout,
          sexp_c_string(scm_ctx, NULL, -1)));
}

/* TODO: draw text */
static sexp scm_func_text(sexp ctx, sexp self, sexp_sint_t n,
    sexp text, sexp x, sexp y) {

  assert(sexp_stringp(text));
  assert(sexp_numberp(x));
  assert(sexp_numberp(y));

  printf("%s\n", sexp_string_data(text));

  return SEXP_VOID;
}

static sexp scm_func_prefer_grpahics_type(sexp ctx, sexp self, sexp_sint_t n,
    sexp s) {

  char *type;
  assert(sexp_stringp(s));

  type = sexp_string_data(s);

  if (strcmp(type, "math") == 0)      graphics_type = GT_MATH;
  else if (strcmp(type, "all") == 0)  graphics_type = GT_ALL;
  else if (strcmp(type, "plot") == 0) graphics_type = GT_PLOT;
  else if (strcmp(type, "game") == 0) graphics_type = GT_GAME;
  else return sexp_make_boolean(0);

  return sexp_make_boolean(1);
}

static sexp scm_func_draw_square(sexp ctx, sexp self, sexp_sint_t n,
    sexp c, sexp x, sexp y, sexp w, sexp h) {

  int r, g, b, a;
  sexp vec = sexp_list_to_vector(ctx, c);
  sexp* data = sexp_vector_data(vec);

  assert(sexp_listp(scm_ctx, c));
  assert(sexp_numberp(x));
  assert(sexp_numberp(y));
  assert(sexp_numberp(w));
  assert(sexp_numberp(h));

  r = sexp_unbox_fixnum(data[0]);
  g = sexp_unbox_fixnum(data[1]);
  b = sexp_unbox_fixnum(data[2]);
  a = sexp_unbox_fixnum(data[3]);

  gui_draw_square(
    sexp_unbox_fixnum(x),
    sexp_unbox_fixnum(y),
    sexp_unbox_fixnum(w),
    sexp_unbox_fixnum(h),
    r, g, b, a
  );

  return SEXP_VOID;
}

static sexp scm_func_draw_line(sexp ctx, sexp self, sexp_sint_t n,
    sexp c, sexp x1, sexp y1, sexp x2, sexp y2) {

  int r, g, b, a;
  sexp vec = sexp_list_to_vector(ctx, c);
  sexp* data = sexp_vector_data(vec);

  assert(sexp_listp(scm_ctx, c));
  assert(sexp_numberp(x1));
  assert(sexp_numberp(y1));
  assert(sexp_numberp(x2));
  assert(sexp_numberp(y2));

  r = sexp_unbox_fixnum(data[0]);
  g = sexp_unbox_fixnum(data[1]);
  b = sexp_unbox_fixnum(data[2]);
  a = sexp_unbox_fixnum(data[3]);

  gui_draw_line(
    sexp_unbox_fixnum(x1),
    sexp_unbox_fixnum(y1),
    sexp_unbox_fixnum(x2),
    sexp_unbox_fixnum(y2),
    r, g, b, a
  );

  return SEXP_VOID;
}

void scm_update_screen(void) {
  sexp s;
  s = sexp_eval_string(scm_ctx, "(update-screen)", -1, NULL);
  print_if_exception(s);
}

static void define_foreign(void) {
  sexp_define_foreign(scm_ctx, sexp_context_env(scm_ctx),
      "text", 3, scm_func_text);

  sexp_define_foreign(scm_ctx, sexp_context_env(scm_ctx),
      "draw-square", 5, scm_func_draw_square);

  sexp_define_foreign(scm_ctx, sexp_context_env(scm_ctx),
      "draw-line", 5, scm_func_draw_line);

  sexp_define_foreign(scm_ctx, sexp_context_env(scm_ctx),
      "prefer-graphics-type", 1, scm_func_prefer_grpahics_type);
}

void init_scheme(void) {
  sexp obj, e;
  assert(scm_ctx == NULL);

  sexp_scheme_init();
  scm_ctx = sexp_load_image("startup-image", 0, 0, SEXP_MAXIMUM_HEAP_SIZE);

  define_foreign();

  obj = sexp_c_string(scm_ctx, "hello.scm", -1);
  e = sexp_load(scm_ctx, obj, NULL);
  print_if_exception(e);
}

void end_scheme(void) {
  sexp_destroy_context(scm_ctx);
  scm_ctx = NULL;
}

