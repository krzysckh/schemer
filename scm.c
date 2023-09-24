#include "schemer.h"

#include <stdio.h>
#include <ctype.h>

#ifdef USE_FFI
#include <dlfcn.h>
#include <ffi.h>

struct ffi_function {
  ffi_cif *cif;
  ffi_type **atypes;
  char *name;
  void *fp;
  int ret_type;
};

struct ffi_function *ffi_functions;
int n_ffi_functions = 0;
#endif

#include <chibi/eval.h>
#include <chibi/gc_heap.h>

#include <raylib.h>
#include <err.h>

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
  else S(v, "ffi")
    include_scm_ffi_scm(scm_ctx);
  else S(v, "core")
    include_scm_core_scm(scm_ctx);
  else S(v, "__CORE__")
    include_third_party_chibi_scheme_lib_init_7_scm(scm_ctx);
  else
    scm_ctx_add(v); /* use as path */

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

/* TODO: free the memory, or figure out the GC */
static sexp scm_func_system(sexp ctx, sexp self, sexp_sint_t n,
    sexp s) {
  FILE *p;
  char *ret = NULL,
       buf[512] = { 0 };
  int allocd = 0, sz = 0, read;

  A(sexp_stringp(s));

  p = popen(sexp_string_data(s), "r");
  if (!p)
    errx(1, "failed to popen(\"%s\", \"r\"", sexp_string_data(s));

  while (!feof(p)) {
    read = fread(buf, 1, 512, p);

    if (read > 0) {
      ret = realloc(ret, allocd + read + 1);
      strcpy(ret + allocd, buf);
      allocd += read;
      ret[allocd] = 0;
    }

    *buf = 0;
    sz += read;
  }

  return sexp_c_string(ctx, ret, sz);
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

static sexp scm_func_ffi_load(sexp ctx, sexp self, sexp_sint_t n,
    sexp path) {
#if USE_FFI
  void *p;
  char *pp, *ps;
  A(sexp_stringp(path));

  ps = strdup(sexp_string_data(path));

  if (is_compiled && is_compiled_in(ps)) {
    pp = compiled_path_to_filename(ps);
    if (pp) {
      free(ps);
      ps = pp;
    }
  }

  p = dlopen(ps, RTLD_NOW);
  if (p == NULL)
    err(errno, "cannot open %s", sexp_string_data(path));

  return sexp_make_cpointer(ctx, SEXP_CPOINTER, p, NULL, 0);
}

static ffi_type *inttotype(int x) {
  switch (x) {
    case 0:  return &ffi_type_void;
    case 1:  return &ffi_type_uint8;
    case 2:  return &ffi_type_sint8;
    case 3:  return &ffi_type_uint16;
    case 4:  return &ffi_type_sint16;
    case 5:  return &ffi_type_uint32;
    case 6:  return &ffi_type_sint32;
    case 7:  return &ffi_type_uint64;
    case 8:  return &ffi_type_sint64;
    case 9:  return &ffi_type_float;
    case 10: return &ffi_type_double;
    case 11: return &ffi_type_pointer;
    default: errx(1, "unknown type");
  }

  /* unreachable */
#else
  errx(1, "compiled without ffi support");
#endif
}

static sexp scm_func_ffi_call(sexp ctx, sexp self, sexp_sint_t n,
    sexp name, sexp args) {
#ifdef USE_FFI
  A(sexp_stringp(name));
  A(sexp_listp(ctx, args));

  typedef union {
    int i;
    unsigned u;
    float f;
    double d;
    void *v;
  } whatever;

  struct ffi_function *f = NULL;
  int i, nargs = sexp_unbox_fixnum(sexp_length_op(ctx, self, n, args));
  void **values = malloc(sizeof(void*) * nargs), *tmp;
  whatever ret;
  sexp a_vec = sexp_list_to_vector_op(ctx, self, n, args);

  for (i = 0; i < n_ffi_functions; ++i) {
    S(ffi_functions[i].name, sexp_string_data(name)) {
      f = &ffi_functions[i];
      break;
    }
  }

  if (!f)
    errx(1, "no such function: %s", sexp_string_data(name));

  for (i = 0; i < nargs; ++i) {
    if (f->atypes[i]->type == FFI_TYPE_POINTER) {
      /* TODO: this sucks, it should not just assume it's a string LOL */
      /* HACK: safety third */
      tmp = malloc(strlen(sexp_string_data(sexp_vector_ref(a_vec, i*2))) + 1);
      strcpy(tmp, sexp_string_data(sexp_vector_ref(a_vec, i*2)));
      values[i] = &tmp;
    } else {
      /* TODO: figure out why i*2 */
      tmp = malloc(f->atypes[i]->size);
      if (f->atypes[i]->type == FFI_TYPE_FLOAT)
        *(float*)tmp = (float)sexp_unbox_fixnum(sexp_vector_ref(a_vec, i*2));
      else if (f->atypes[i]->type == FFI_TYPE_DOUBLE)
        *(double*)tmp = (double)sexp_unbox_fixnum(sexp_vector_ref(a_vec, i*2));
      else
        *(sexp_sint_t*)tmp = (sexp_sint_t)
          sexp_unbox_fixnum(sexp_vector_ref(a_vec, i*2));
      values[i] = tmp;
    }
  }

  ffi_call(f->cif, f->fp, &ret.v, values);

  switch (f->ret_type) {
    case 0:  return SEXP_VOID;
    case 1:  return sexp_make_fixnum((uint8_t)ret.u);
    case 2:  return sexp_make_fixnum((int8_t)ret.i);
    case 3:  return sexp_make_fixnum((uint16_t)ret.u);
    case 4:  return sexp_make_fixnum((int16_t)ret.i);
    case 5:  return sexp_make_fixnum((uint32_t)ret.u);
    case 6:  return sexp_make_fixnum((int32_t)ret.i);
    case 7:  return sexp_make_fixnum((uint64_t)ret.u);
    case 8:  return sexp_make_fixnum((int64_t)ret.i);
    case 9:  return sexp_make_flonum(ctx, (float)ret.f);
    case 10: return sexp_make_flonum(ctx, (double)ret.d);
    case 11: return sexp_make_cpointer(ctx, SEXP_CPOINTER, ret.v, NULL, 0);
    default: errx(1, "unreachable");
  }
#else
  errx(1, "compiled without ffi support");
#endif
}

static sexp scm_func_ffi_define(sexp ctx, sexp self, sexp_sint_t n,
    sexp lib, sexp type, sexp name, sexp args, sexp scm_name) {
#if USE_FFI
  void *l, *f;
  char *nam, *snam;
  ffi_type *t, **a;
  ffi_cif *cif = malloc(sizeof(ffi_cif));
  int i, nargs;
  sexp a_vec;

  A(sexp_cpointerp(lib));
  A(sexp_numberp(type));
  A(sexp_stringp(name));
  A(sexp_listp(ctx, args));
  A(sexp_stringp(scm_name));

  l = sexp_cpointer_value(lib);
  t = inttotype(sexp_unbox_fixnum(type));
  nam = sexp_string_data(name);
  nargs = sexp_unbox_fixnum(sexp_length_op(ctx, self, n, args));
  a = malloc(sizeof(ffi_type*) * nargs);
  snam = sexp_string_data(scm_name);
  a_vec = sexp_list_to_vector(ctx, args);

  for (i = 0; i < nargs; ++i) {
    a[i] = inttotype(sexp_unbox_fixnum(sexp_vector_ref(a_vec, i)));
  }

  f = dlsym(l, nam);

  ffi_prep_cif(cif, FFI_DEFAULT_ABI, (unsigned)nargs, t, a);

  ffi_functions = realloc(ffi_functions, sizeof(struct ffi_function) *
      (1 + n_ffi_functions));
  ffi_functions[n_ffi_functions].cif = cif;
  ffi_functions[n_ffi_functions].fp = f;
  ffi_functions[n_ffi_functions].ret_type = sexp_unbox_fixnum(type);
  ffi_functions[n_ffi_functions].name = strdup(snam);
  ffi_functions[n_ffi_functions].atypes = a;
  n_ffi_functions++;

  return SEXP_VOID;
#else
  errx(1, "compiled without ffi support");
#endif
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
      "ffi-load", 1, scm_func_ffi_load);

  sexp_define_foreign(scm_ctx, sexp_context_env(scm_ctx),
      "ffi-define", 5, scm_func_ffi_define);

  sexp_define_foreign(scm_ctx, sexp_context_env(scm_ctx),
      "ffi-call", 2, scm_func_ffi_call);

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

