#include "schemer.h"

#include <stdio.h>
#include <raylib.h>

void gui_draw_square(int x, int y, int w, int h, int r, int g, int b, int a) {
  DrawRectangle(x, y, w, h, CLITERAL(Color){r, g, b, a});
}

void gui_draw_line(int x1, int y1, int x2, int y2, int r, int g, int b,
    int a) {
  DrawLine(x1, y1, x2, y2, CLITERAL(Color){r, g, b, a});
}

void init_gui(void) {
  /* TODO: define-width, define-height etc. */
  const int width = 640,
            height = 480,
            targetfps = 60;
  char *title = "schemer gui";

  InitWindow(width, height, title);
  SetTargetFPS(targetfps);

}

void run_gui(void) {
  while (!WindowShouldClose()) {
    BeginDrawing();
    {
      ClearBackground(WHITE);
      scm_update_screen();
    }
    EndDrawing();
  }
}

void end_gui(void) {
  CloseWindow();
}
