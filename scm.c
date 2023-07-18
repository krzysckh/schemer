#include "schemer.h"

#include <stdio.h>
#include <err.h>
#include <ctype.h>

#include <chibi/eval.h>
#include <chibi/gc_heap.h>

#include <raylib.h>

extern Font default_font;

sexp scm_ctx = NULL;

static Color background_color = WHITE;
static Color font_color = BLACK;

static Texture2D *loaded_textures = NULL;
static int n_loaded_textures = 0;

static int last_pressed_key = -1;

static char** scheme_args = NULL;
static int n_scheme_args  = 0;

extern int is_compiled;

int print_if_exception(sexp s) {
  if (sexp_exceptionp(s)) {
    sexp_print_exception(scm_ctx, s,
        sexp_make_output_port(scm_ctx, stdout,
          sexp_c_string(scm_ctx, NULL, -1)));
    return 1;
  }

  return 0;
}

void add_scheme_arg(char *s) {
  scheme_args = realloc(scheme_args, sizeof(char*) * (n_scheme_args + 1));
  scheme_args[n_scheme_args++] = s; /* don't copy strings, just addresses */
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

static char *compiled_path_to_filename(char *path) {
  char *c, *tnam, *nam;
  int sz;
  FILE *f;

  c = get_contents_of(path);
  if (c) {
    sz = get_length_of(path);
    tnam = tmpnam(NULL);
    nam = malloc(strlen(tnam) + 8);
    snprintf(nam, strlen(tnam) + 8, "%s%s", tnam, GetFileExtension(path));
    /* a hack, because FUCKING raylib expects a filename with extension */
    f = fopen(nam, "wb");
    fwrite(c, 1, sz, f);
    free(c);
    fclose(f);

    return nam;
  }

  warnx("schemer warning: loading file %s in compiled mode "
      "(maybe you forgot to (define-resource) it?)", path);
  return NULL;
}

/* TODO: keep track of added files. don't evaluate files that have been already
 * evaluated */
void scm_ctx_add(char *s) {
  sexp obj, e;
  /* if schemer build */
  char *p;

  if (is_compiled && is_compiled_in(s)) {
    compiled_include(scm_ctx, s);
    return;
  } else {
    obj = sexp_c_string(scm_ctx, s, -1);
    e = sexp_load(scm_ctx, obj, NULL);
  }

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
      C1500FONT_FONT_SIZE, 0.f, font_color);

  return SEXP_VOID;
}

static sexp scm_func_use(sexp ctx, sexp self, sexp_sint_t n,
    sexp s) {

  char *v;
  A(sexp_stringp(s));

  v = sexp_string_data(s);

  S(v, "plot")
    include_scm_plot_scm(scm_ctx);
  else S(v, "game2d")
    include_scm_game2d_scm(scm_ctx);
  else S(v, "make")
    include_scm_make_scm(scm_ctx);
  else S(v, "colors")
    include_scm_colors_scm(scm_ctx);
  else S(v, "click")
    include_scm_click_scm(scm_ctx);
  else S(v, "shapes")
    include_scm_shapes_scm(scm_ctx);
  else S(v, "ui")
    include_scm_ui_scm(scm_ctx);
  else S(v, "core")
    include_scm_core_scm(scm_ctx);
  else S(v, "__CORE__")
    include_third_party_chibi_scheme_lib_init_7_scm(scm_ctx);
  else {
    sexp_warn(scm_ctx, "using 'use' path as a regular path", NULL);
    scm_ctx_add(v); /* use as path */
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
  extern int win_width, win_height;
  int w, h;
  w = GetScreenWidth();
  h = GetScreenHeight();
  return sexp_list2(ctx,
      sexp_make_fixnum(w == 0 ? win_width : w),
      sexp_make_fixnum(w == 0 ? win_height : h)); /* woah */
}

static sexp scm_func_load_image(sexp ctx, sexp self, sexp_sint_t n,
    sexp path) {
  sexp ptr;
  char *ps, *p;
  A(sexp_stringp(path));

  ps = strdup(sexp_string_data(path));

  if (is_compiled && is_compiled_in(ps)) {
    p = compiled_path_to_filename(ps);
    if (p) {
      free(ps);
      ps = p;
    }
  }

  loaded_textures = realloc(loaded_textures,
      sizeof(Texture2D) * (n_loaded_textures + 1));

  loaded_textures[n_loaded_textures] = LoadTexture(ps);

  ptr = sexp_make_cpointer(ctx, SEXP_CPOINTER,
      &loaded_textures[n_loaded_textures], NULL, 0);

  n_loaded_textures++;

  free(ps);
  return ptr;
}

static sexp scm_func_show_image(sexp ctx, sexp self, sexp_sint_t n,
    sexp image, sexp x, sexp y) {
  A(sexp_cpointerp(image))
  A(sexp_fixnump(x))
  A(sexp_fixnump(y))

  gui_draw_image(sexp_cpointer_value(image), sexp_unbox_fixnum(x),
      sexp_unbox_fixnum(y));

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

  return sexp_make_boolean(
      IsMouseButtonPressed(sexp_unbox_fixnum(button))
      || IsMouseButtonDown(sexp_unbox_fixnum(button)));
}

static sexp scm_func_get_mouse_pos(sexp ctx, sexp self, sexp_sint_t n) {
  return sexp_list2(ctx,
    sexp_make_fixnum(GetMouseX()),
    sexp_make_fixnum(GetMouseY()));
}

static sexp scm_func_get_key_pressed(sexp ctx, sexp self, sexp_sint_t n) {
  int c = GetKeyPressed();

  if (c == 0 && IsKeyDown(last_pressed_key))
    c = last_pressed_key;

  if (c) {
    last_pressed_key = c;
    return sexp_make_character(c);
  } else
    return sexp_make_boolean(0);
}

static sexp scm_func_is_key_pressed(sexp ctx, sexp self, sexp_sint_t n,
    sexp c) {
  A(sexp_fixnump(c) || sexp_charp(c) || sexp_stringp(c));

  if (sexp_fixnump(c))
    return sexp_make_boolean(IsKeyDown(toupper(sexp_unbox_fixnum(c))));
  else if (sexp_charp(c))
    return sexp_make_boolean(IsKeyDown(toupper(sexp_unbox_character(c))));
  else
    return sexp_make_boolean(IsKeyDown(toupper(sexp_string_data(c)[0])));
}

static sexp scm_func_system(sexp ctx, sexp self, sexp_sint_t n,
    sexp s) {
  A(sexp_stringp(s));

  return sexp_make_fixnum(system(sexp_string_data(s)));
}

static sexp scm_func_dont_init_graphics(sexp ctx, sexp self, sexp_sint_t n) {
  extern int init_graphics;
  init_graphics = 0;

  return SEXP_VOID;
}

static sexp scm_func_set_window_size(sexp ctx, sexp self, sexp_sint_t n,
    sexp w, sexp h) {
  extern int win_width, win_height;

  A(sexp_fixnump(w));
  A(sexp_fixnump(h));

  SetWindowSize(sexp_unbox_fixnum(w), sexp_unbox_fixnum(h));
  win_width = sexp_unbox_fixnum(w);
  win_height = sexp_unbox_fixnum(h);

  return SEXP_VOID;
}

static sexp scm_func_set_window_resizable(sexp ctx, sexp self, sexp_sint_t n,
    sexp b) {
  extern int win_resizable;
  int b_u;

  A(sexp_booleanp(b));
  win_resizable = b_u = sexp_unbox_boolean(b);

  if (b_u)
    SetWindowState(FLAG_WINDOW_RESIZABLE);
  else
    SetWindowState(0);

  return SEXP_VOID;
}

static sexp scm_func_compiler_add_resource(sexp ctx, sexp self, sexp_sint_t n,
    sexp path) {
  A(sexp_stringp(path));

  schemer_add_resource(sexp_string_data(path));

  return SEXP_VOID;
}

static sexp scm_func_rand(sexp ctx, sexp self, sexp_sint_t n) {
  return sexp_make_flonum(ctx, ((double)rand())/RAND_MAX);
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
      "get-key-pressed", 0, scm_func_get_key_pressed);

  sexp_define_foreign(scm_ctx, sexp_context_env(scm_ctx),
      "is-key-pressed", 1, scm_func_is_key_pressed);

  sexp_define_foreign(scm_ctx, sexp_context_env(scm_ctx),
      "system", 1, scm_func_system);

  sexp_define_foreign(scm_ctx, sexp_context_env(scm_ctx),
      "dont-init-graphics", 0, scm_func_dont_init_graphics);

  sexp_define_foreign(scm_ctx, sexp_context_env(scm_ctx),
      "set-window-size", 2, scm_func_set_window_size);

  sexp_define_foreign(scm_ctx, sexp_context_env(scm_ctx),
      "set-window-resizable", 1, scm_func_set_window_resizable);

  sexp_define_foreign(scm_ctx, sexp_context_env(scm_ctx),
      "compiler-add-resource", 1, scm_func_compiler_add_resource);

  sexp_define_foreign(scm_ctx, sexp_context_env(scm_ctx),
      "rand", 0, scm_func_rand);

  sexp_define_foreign(scm_ctx, sexp_context_env(scm_ctx),
      "use", 1, scm_func_use);
}

static void define_argv(void) {
  extern char*  argv0;
  extern char** scheme_args;
  extern int    n_scheme_args;

  sexp l;
  int i;

  l = sexp_list1(scm_ctx, sexp_c_string(scm_ctx, argv0, -1));
  for (i = 0; i < n_scheme_args; ++i)
    l = sexp_append2(scm_ctx, l,
        sexp_list1(scm_ctx, sexp_c_string(scm_ctx, scheme_args[i], -1)));

  sexp_env_define(scm_ctx, sexp_context_env(scm_ctx),
      sexp_string_to_symbol(scm_ctx, sexp_c_string(scm_ctx, "argv", -1)), l);
}

void scm_run_onload(void) {
  sexp s;

  s = sexp_eval_string(scm_ctx, "(on-load)", -1, NULL);
  print_if_exception(s);
}

void init_scheme(char *path) {
  A(scm_ctx == NULL);
  srand(time(0));

  sexp_scheme_init();
  scm_ctx = sexp_make_eval_context(NULL, NULL, NULL, 0, 0);
  sexp_load_standard_env(scm_ctx, NULL, SEXP_SEVEN);
  sexp_load_standard_ports(scm_ctx, NULL, stdin, stdout, stderr, 1);

  define_foreign();
  define_argv();

  if (path)
    scm_ctx_add(path);
  else {
    /* this is so cool */
    sexp_eval_string(
      scm_ctx,
      "(define update-screen (lambda () \
      (text \"no base path given or update-screen not defined\" 0 0)))",
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

