#define PLOT_SCM_PATH   "scm/plot.scm"
#define COLORS_SCM_PATH "scm/colors.scm"
#define SHAPES_SCM_PATH "scm/shapes.scm"
#define CORE_SCM_PATH   "scm/core.scm"

/* unifont.c */
#define UNIFONT_FONT_SIZE 20
#define UNIFONT     __third_party_unifont_15_0_06_ttf
#define UNIFONT_LEN __third_party_unifont_15_0_06_ttf_len

extern unsigned int __third_party_unifont_15_0_06_ttf_len;
extern unsigned char __third_party_unifont_15_0_06_ttf[];

#include <chibi/sexp.h>
/* scm.c */
void init_scheme(char *path);
void end_scheme(void);
void scm_update_screen(void);
int print_if_exception(sexp s);

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
