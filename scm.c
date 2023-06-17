#include "schemer.h"

#include <stdio.h>
#include <err.h>

#include <chibi/eval.h>
#include <chibi/gc_heap.h>

#include <raylib.h>

extern Font default_font;

static sexp scm_ctx = NULL;

static Color background_color = WHITE;
static Color font_color = BLACK;

static Texture2D *loaded_textures = NULL;
static int n_loaded_textures = 0;

int print_if_exception(sexp s) {
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

  A(sexp_listp(scm_ctx, l));
  A(sexp_vector_length(vec) == 4 || sexp_vector_length(vec) == 3);

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

  A(sexp_stringp(text));
  A(sexp_numberp(x));
  A(sexp_numberp(y));

  DrawTextEx(default_font, sexp_string_data(text),
      (Vector2){(int)sexp_unbox_fixnum(x), (int)sexp_unbox_fixnum(y)},
      UNIFONT_FONT_SIZE, 0.f, font_color);

  return SEXP_VOID;
}

static sexp scm_func_use(sexp ctx, sexp self, sexp_sint_t n,
    sexp s) {

  char *v;
  A(sexp_stringp(s));

  v = sexp_string_data(s);

  if (strcmp(v, "plot") == 0)
    include_scm_plot_scm(scm_ctx);
  else if (strcmp(v, "colors") == 0)
    include_scm_colors_scm(scm_ctx);
  else if (strcmp(v, "click") == 0)
    include_scm_click_scm(scm_ctx);
  else if (strcmp(v, "shapes") == 0)
    include_scm_shapes_scm(scm_ctx);
  else if (strcmp(v, "core") == 0)
    include_scm_core_scm(scm_ctx);
  else if (strcmp(v, "__CORE__") == 0)
    include_chibi_scheme_lib_init_7_scm(scm_ctx);
  else {
    sexp_warn(scm_ctx, "using 'use' path as a regular path", NULL);
    ctx_add(v); /* use as path */
  }

  return SEXP_VOID;
}

static sexp scm_func_draw_square(sexp ctx, sexp self, sexp_sint_t n,
    sexp c, sexp x, sexp y, sexp w, sexp h) {
  int r, g, b, a;

  A(sexp_numberp(x));
  A(sexp_numberp(y));
  A(sexp_numberp(w));
  A(sexp_numberp(h));
  list2rgba(ctx, c, &r, &g, &b, &a);

  gui_draw_square(
    sexp_unbox_fixnum(x),
    sexp_unbox_fixnum(y),
    sexp_unbox_fixnum(w),
    sexp_unbox_fixnum(h),
    r, g, b, a
  );

  return sexp_list2(ctx,
    sexp_list2(ctx, x, y),
    sexp_list2(ctx, sexp_add(ctx, x, w), sexp_add(ctx, y, h)));
}

static sexp scm_func_draw_line(sexp ctx, sexp self, sexp_sint_t n,
    sexp c, sexp x1, sexp y1, sexp x2, sexp y2) {
  int r, g, b, a;

  A(sexp_numberp(x1));
  A(sexp_numberp(y1));
  A(sexp_numberp(x2));
  A(sexp_numberp(y2));
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

static sexp scm_func_load_image(sexp ctx, sexp self, sexp_sint_t n,
    sexp path) {
  sexp ptr;

  A(sexp_stringp(path));

  /* TODO: register the data somewhere it can be stored and added to the final
   * executable via schemer build */
  loaded_textures = realloc(loaded_textures,
      sizeof(Texture2D) * (n_loaded_textures + 1));

  loaded_textures[n_loaded_textures] = LoadTexture(sexp_string_data(path));

  ptr = sexp_make_cpointer(ctx, SEXP_CPOINTER,
      &loaded_textures[n_loaded_textures], NULL, 0);

  n_loaded_textures++;

  return ptr;
}

static sexp scm_func_show_image(sexp ctx, sexp self, sexp_sint_t n,
    sexp image, sexp x, sexp y) {
  A(sexp_cpointerp(image))
  A(sexp_fixnump(x))
  A(sexp_fixnump(y))

  gui_draw_image(sexp_cpointer_value(image), 0, 0);

  return SEXP_VOID;
}

static sexp scm_func_resize_image(sexp ctx, sexp self, sexp_sint_t n,
    sexp image, sexp w, sexp h) {
  Image img;

  A(sexp_cpointerp(image))
  A(sexp_fixnump(w))
  A(sexp_fixnump(h))

  img = LoadImageFromTexture(*(Texture2D*)sexp_cpointer_value(image));
  ImageResize(&img, sexp_unbox_fixnum(w), sexp_unbox_fixnum(h));

  *(Texture2D*)sexp_cpointer_value(image) = LoadTextureFromImage(img);

  return SEXP_VOID;
}

static sexp scm_func_is_mouse_pressed(sexp ctx, sexp self, sexp_sint_t n,
    sexp button) {
  A(sexp_fixnump(button));

  return sexp_make_boolean(IsMouseButtonPressed(sexp_unbox_fixnum(button)));
}

static sexp scm_func_get_mouse_pos(sexp ctx, sexp self, sexp_sint_t n) {
  return sexp_list2(ctx,
    sexp_make_fixnum(GetMouseX()),
    sexp_make_fixnum(GetMouseY()));
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
      "load-image", 1, scm_func_load_image);

  sexp_define_foreign(scm_ctx, sexp_context_env(scm_ctx),
      "show-image", 3, scm_func_show_image);

  sexp_define_foreign(scm_ctx, sexp_context_env(scm_ctx),
      "resize-image", 3, scm_func_resize_image);

  sexp_define_foreign(scm_ctx, sexp_context_env(scm_ctx),
      "is-mouse-pressed", 1, scm_func_is_mouse_pressed);

  sexp_define_foreign(scm_ctx, sexp_context_env(scm_ctx),
      "get-mouse-pos", 0, scm_func_get_mouse_pos);

  sexp_define_foreign(scm_ctx, sexp_context_env(scm_ctx),
      "use", 1, scm_func_use);
}

void init_scheme(char *path) {
  A(scm_ctx == NULL);

  sexp_scheme_init();
  scm_ctx = sexp_make_eval_context(NULL, NULL, NULL, 0, 0);
  sexp_load_standard_env(scm_ctx, NULL, SEXP_SEVEN);
  sexp_load_standard_ports(scm_ctx, NULL, stdin, stdout, stderr, 1);

  define_foreign();

  if (path)
    ctx_add(path);
  else {
    /* this is so cool */
    sexp_eval_string(
      scm_ctx,
      "(define update-screen (lambda () (text \"no base path given\" 0 0)))",
      -1,
      NULL
    );
  }
}

void end_scheme(void) {
  free(loaded_textures);

  sexp_destroy_context(scm_ctx);
  scm_ctx = NULL;
}

