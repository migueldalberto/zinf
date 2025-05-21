#include <raylib.h>

#include <stdbool.h>

#define LARGURA 1200
#define ALTURA 800

typedef enum { NORTE, SUL, LESTE, OESTE} Orientacao;

typedef struct {
  int vida;
  int pontuacao;
  Vector2 posicao;
  Orientacao orientacao;
  bool espada;
  Texture2D texturas[4];
} Jogador;

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
