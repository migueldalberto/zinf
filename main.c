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

typedef struct {
  Jogador jogador;
  int nivel;
} Jogo;


void DesenharStatus(Jogo j) {
  DrawRectangle(0, 0, LARGURA, 60, BLACK);
  DrawText(
	   TextFormat("vida: %d \tnível: %d \tpontuação: %d",
		      j.jogador.vida,
		      j.nivel,
		      j.jogador.pontuacao),
	   16,
	   16,
	   32,
	   WHITE
	   );
}

int main () {
  InitWindow(LARGURA, ALTURA, "ZINF");

  Jogador jogador = {
    .vida = 3,
    .pontuacao = 0,
    .posicao = (Vector2){ LARGURA / 2, ALTURA / 2},
    .orientacao = LESTE,
    .espada = false,
    .texturas = {
      LoadTexture("assets/jogador-norte.png"), LoadTexture("assets/jogador-sul.png"),
      LoadTexture("assets/jogador-leste.png"), LoadTexture("assets/jogador-oeste.png"),
    }
  };

  Jogo jogo = { .jogador = jogador, .nivel = 0 };

  SetTargetFPS(30);

  while(!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(WHITE);
    DesenharStatus(jogo);

    EndDrawing();
  }

  CloseWindow();
}
