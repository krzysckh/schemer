#include "schemer.h"

#include <stdio.h>
#include <assert.h>

#include <chibi/eval.h>
#include <chibi/gc_heap.h>

static sexp scm_ctx = NULL;

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

