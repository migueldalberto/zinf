#include <raylib.h>

#define LARGURA 1200
#define ALTURA 800

int main () {
  InitWindow(LARGURA, ALTURA, "ZINF");

  SetTargetFPS(30);

  while(!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(WHITE);
    EndDrawing();
  }

  CloseWindow();
}
