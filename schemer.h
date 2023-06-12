/* unifont.c */
#define UNIFONT_FONT_SIZE 20
#define UNIFONT     __third_party_unifont_15_0_06_ttf
#define UNIFONT_LEN __third_party_unifont_15_0_06_ttf_len

extern unsigned int __third_party_unifont_15_0_06_ttf_len;
extern unsigned char __third_party_unifont_15_0_06_ttf[];

/* scm.c */
void init_scheme(void);
void end_scheme(void);
void scm_update_screen(void);

/* gui.c */
void init_gui(void);
void end_gui(void);
void run_gui(void);

/* gui.c tunnel funcitons */
void gui_draw_square(int, int, int, int, int, int, int, int);
void gui_draw_line(int, int, int, int, int, int, int, int);

#define gui_draw_square(x,y,w,h,r,g,b,a) \
  DrawRectangle(x, y, w, h, (Color){r, g, b, a})

#define gui_draw_line(x1,y1,x2,y2,r,g,b,a) \
  DrawLine(x1, y1, x2, y2, (Color){r, g, b, a})

#define gui_clear_background(r,g,b,a) \
  ClearBackground((Color){r, g, b, a});
