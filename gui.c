#include "schemer.h"

#include <stdio.h>
#include <raylib.h>

Font default_font;
int init_graphics = 1;

int win_width = 640;
int win_height = 480;
int win_resizable = 1;
int targetfps = 30;

void init_gui(void) {
  AOR(init_graphics)
    return;

  char *title = "schemer gui";

  InitWindow(win_width, win_height, title);
  SetTargetFPS(targetfps);

  default_font = LoadFontFromMemory(".ttf", C1500FONT, C1500FONT_LEN,
      C1500FONT_FONT_SIZE, NULL, 0);

  if (win_resizable)
    SetWindowState(FLAG_WINDOW_RESIZABLE);

  SetWindowMinSize(320, 240);

  scm_run_onload();
}

void run_gui(void) {
  while (!WindowShouldClose() && init_graphics) {
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
