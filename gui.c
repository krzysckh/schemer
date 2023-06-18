#include "schemer.h"

#include <stdio.h>
#include <raylib.h>

Font default_font;
int init_graphics = 1;

void init_gui(void) {
  /* TODO: define-width, define-height etc. */
  AOR(init_graphics)
    return;

  const int width = 640,
            height = 480,
            targetfps = 30;
  char *title = "schemer gui";

  InitWindow(width, height, title);
  SetTargetFPS(targetfps);

  default_font = LoadFontFromMemory(".ttf", UNIFONT, UNIFONT_LEN,
      UNIFONT_FONT_SIZE, NULL, 0);

  SetWindowState(FLAG_WINDOW_RESIZABLE);
  SetWindowMinSize(320, 240);
}

void run_gui(void) {
  AOR(init_graphics)
    return;
  while (!WindowShouldClose()) {
    BeginDrawing();
    {
      scm_update_screen();
    }
    EndDrawing();
  }
}

void end_gui(void) {
  AOR(init_graphics)
    return;
  CloseWindow();
}
