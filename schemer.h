
/* scm.c */
void init_scheme(void);
void end_scheme(void);
void scm_update_screen(void);

/* gui.c */
void init_gui(void);
void end_gui(void);
void run_gui(void);

/* gui.c tunnel funcitons */
void gui_draw_square(int x, int y, int w, int h, int r, int g, int b, int a);
