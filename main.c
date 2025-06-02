#include <raylib.h>

#include <stdbool.h>

#include "sprite.h"

#define LARGURA 1200
#define ALTURA 800
#define ESCALA 0.2

typedef enum { NORTE, SUL, LESTE, OESTE} Orientacao;

typedef struct {
  int vida;
  int pontuacao;
  Vector2 posicao;
  Orientacao orientacao;
  bool espada;
  Sprite sprite;
} Jogador;

typedef struct {
  Jogador jogador;
  int nivel;
} Jogo;

void AtualizarSprite(Jogador* j) {
  j->sprite.src = (Rectangle) {
    j->sprite.frameAtual * (j->sprite.textura->width / j->sprite.numeroDeFrames),
    j->orientacao * (j->sprite.textura->height / 4),
    (j->sprite.textura->width / j->sprite.numeroDeFrames),
    (j->sprite.textura->height / 4)
  };

  j->sprite.dst = (Rectangle){
    j->posicao.x,
    j->posicao.y,
    (j->sprite.textura->width / j->sprite.numeroDeFrames) * ESCALA,
    (j->sprite.textura->height / 4) * ESCALA
  };
}

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

  Texture2D texturaDoJogador = LoadTexture("./assets/jogador.png");

  Jogador jogador = {
    .vida = 3,
    .pontuacao = 0,
    .posicao = (Vector2){ LARGURA / 2, ALTURA / 2},
    .orientacao = LESTE,
    .espada = false,
    .sprite = {
      .textura = &texturaDoJogador,
      .numeroDeFrames = 1,
      .frameAtual = 0
    }
  };

  Jogo jogo = { .jogador = jogador, .nivel = 0 };

  SetTargetFPS(30);

  while(!WindowShouldClose()) {
    AtualizarSprite(&jogador);
    
    BeginDrawing();
    ClearBackground(WHITE);
    DesenharSprite(jogador.sprite);
    DesenharStatus(jogo);

    EndDrawing();
  }

  CloseWindow();
}
