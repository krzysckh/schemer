#include "schemer.h"

#include <stdio.h>
#include <assert.h>

#include <chibi/eval.h>
#include <chibi/gc_heap.h>

#include <raylib.h>

extern Font default_font;

static sexp scm_ctx = NULL;

static Color background_color = WHITE;
static Color font_color = BLACK;

static int print_if_exception(sexp s) {
  if (sexp_exceptionp(s)) {
    sexp_print_exception(scm_ctx, s,
        sexp_make_output_port(scm_ctx, stdout,
          sexp_c_string(scm_ctx, NULL, -1)));
    return 1;
  }

  return 0;
}

static void list2rgba(sexp ctx, sexp l, int *r, int *g, int *b, int *a) {
  sexp vec = sexp_list_to_vector(ctx, l);
  sexp* data = sexp_vector_data(vec);

  assert(sexp_listp(scm_ctx, l));
  assert(sexp_vector_length(vec) == 4 ||
         sexp_vector_length(vec) == 3);

  *r = sexp_unbox_fixnum(data[0]);
  *g = sexp_unbox_fixnum(data[1]);
  *b = sexp_unbox_fixnum(data[2]);
  *a = sexp_vector_length(vec) == 4 ? sexp_unbox_fixnum(data[3]) : 255;
}

/* TODO: keep track of added files. don't evaluate files that have been already
 * evaluated */
static void ctx_add(const char *s) {
  sexp obj, e;

  obj = sexp_c_string(scm_ctx, s, -1);
  e = sexp_load(scm_ctx, obj, NULL);

  if (print_if_exception(e))
    exit(1);
}

static sexp scm_func_text(sexp ctx, sexp self, sexp_sint_t n,
    sexp text, sexp x, sexp y) {

  assert(sexp_stringp(text));
  assert(sexp_numberp(x));
  assert(sexp_numberp(y));

  DrawTextEx(default_font, sexp_string_data(text),
      (Vector2){(int)sexp_unbox_fixnum(x), (int)sexp_unbox_fixnum(y)},
      UNIFONT_FONT_SIZE, 0.f, font_color);

  return SEXP_VOID;
}

static sexp scm_func_use(sexp ctx, sexp self, sexp_sint_t n,
    sexp s) {

  char *v;
  assert(sexp_stringp(s));

  v = sexp_string_data(s);

  if (strcmp(v, "plot") == 0)
    ctx_add(PLOT_SCM_PATH);
  else if (strcmp(v, "colors") == 0)
    ctx_add(COLORS_SCM_PATH);
  else if (strcmp(v, "shapes") == 0)
    ctx_add(SHAPES_SCM_PATH);
  else if (strcmp(v, "core") == 0)
    ctx_add(CORE_SCM_PATH);
  else {
    sexp_warn(scm_ctx, "using 'use' path as a regular path", NULL);
    ctx_add(v); /* use as path */
  }

  return SEXP_VOID;
}

static sexp scm_func_draw_square(sexp ctx, sexp self, sexp_sint_t n,
    sexp c, sexp x, sexp y, sexp w, sexp h) {
  int r, g, b, a;

  assert(sexp_numberp(x));
  assert(sexp_numberp(y));
  assert(sexp_numberp(w));
  assert(sexp_numberp(h));
  list2rgba(ctx, c, &r, &g, &b, &a);

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

  assert(sexp_numberp(x1));
  assert(sexp_numberp(y1));
  assert(sexp_numberp(x2));
  assert(sexp_numberp(y2));
  list2rgba(ctx, c, &r, &g, &b, &a);

  gui_draw_line(
    sexp_unbox_fixnum(x1),
    sexp_unbox_fixnum(y1),
    sexp_unbox_fixnum(x2),
    sexp_unbox_fixnum(y2),
    r, g, b, a
  );

  return SEXP_VOID;
}

static sexp scm_func_define_background_color(sexp ctx, sexp self, sexp_sint_t n,
    sexp c) {
  int r, g, b, a;

  list2rgba(ctx, c, &r, &g, &b, &a);
  background_color = (Color){r, g, b, a};
  return SEXP_VOID;
}

static sexp scm_func_define_font_color(sexp ctx, sexp self, sexp_sint_t n,
    sexp c) {
  int r, g, b, a;

  list2rgba(ctx, c, &r, &g, &b, &a);
  font_color = (Color){r, g, b, a};
  return SEXP_VOID;
}

static sexp scm_func_get_window_size(sexp ctx, sexp self, sexp_sint_t n) {
  return sexp_list2(ctx,
      sexp_make_fixnum(GetScreenWidth()),
      sexp_make_fixnum(GetScreenHeight())); /* woah */
}

void scm_update_screen(void) {
  sexp s;

  gui_clear_background(background_color.r, background_color.g,
      background_color.b, background_color.a);

  s = sexp_eval_string(scm_ctx, "(update-screen)", -1, NULL);
  print_if_exception(s);
}

static void define_foreign(void) {
  sexp_define_foreign(scm_ctx, sexp_context_env(scm_ctx),
      "text", 3, scm_func_text);

  sexp_define_foreign(scm_ctx, sexp_context_env(scm_ctx),
      "define-background-color", 1, scm_func_define_background_color);

  sexp_define_foreign(scm_ctx, sexp_context_env(scm_ctx),
      "define-font-color", 1, scm_func_define_font_color);

  sexp_define_foreign(scm_ctx, sexp_context_env(scm_ctx),
      "draw-square", 5, scm_func_draw_square);

  sexp_define_foreign(scm_ctx, sexp_context_env(scm_ctx),
      "draw-line", 5, scm_func_draw_line);

  sexp_define_foreign(scm_ctx, sexp_context_env(scm_ctx),
      "get-window-size", 0, scm_func_get_window_size);

  sexp_define_foreign(scm_ctx, sexp_context_env(scm_ctx),
      "use", 1, scm_func_use);
}

void init_scheme(void) {
  sexp obj, e;
  assert(scm_ctx == NULL);

  sexp_scheme_init();
  scm_ctx = sexp_load_image("startup-image.img", 0, 0, SEXP_MAXIMUM_HEAP_SIZE);

  define_foreign();

  obj = sexp_c_string(scm_ctx, "hello.scm", -1);
  e = sexp_load(scm_ctx, obj, NULL);
  if (print_if_exception(e))
    exit(1);
}

void end_scheme(void) {
  sexp_destroy_context(scm_ctx);
  scm_ctx = NULL;
}

