#include <chibi/sexp.h>

#define C1500FONT_FONT_SIZE 20
#define C1500FONT     __third_party_canada1500_ttf
#define C1500FONT_LEN __third_party_canada1500_ttf_len

extern unsigned int C1500FONT_LEN;
extern unsigned char C1500FONT[];

/* res-handler.c || custom res-handler.c from schemer build */
int is_compiled_in(char *path);
void compiled_include(sexp ctx, char *s);
char *get_contents_of(char *path);
int get_length_of(char *path);

/* compiler.c */
void schemer_add_resource(char *path);
void schemer_compile(void);
void schemer_free_resources(void);
void filename_to_cfun(char *path);

/* scm.c */
void init_scheme(char *path);
void end_scheme(void);
void scm_update_screen(void);
void scm_ctx_add(char *s);
void scm_run_onload(void);
int print_if_exception(sexp s);
void any_to_c(char *path, char *opath);
void add_scheme_arg(char *s);

/* gui.c */
void init_gui(void);
void end_gui(void);
void run_gui(void);

/* gui.c tunnel funcitons */

#define gui_draw_square(x,y,w,h,r,g,b,a) \
  DrawRectangle(x, y, w, h, (Color){r, g, b, a})

#define gui_draw_line(x1,y1,x2,y2,r,g,b,a) \
  DrawLine(x1, y1, x2, y2, (Color){r, g, b, a})

#define gui_draw_image(imgptr,x,y) \
  DrawTexture(*((Texture2D*)(imgptr)), x, y, WHITE)

#define gui_clear_background(r,g,b,a) \
  ClearBackground((Color){r, g, b, a});

/* very cool assert */
#define A(x) if (!(x)) { errx(1, "assertion failed: %s\n", #x); }
#define AOR(x) if (!(x))

#define S(a, b) if (strcmp((a), (b)) == 0)

