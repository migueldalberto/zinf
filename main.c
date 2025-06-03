#include <raylib.h>

#include <stdbool.h>

#include "defs.h"
#include "sprite.h"

typedef enum { OESTE, LESTE, NORTE, SUL } Orientacao;

typedef struct {
  int vida;
  int pontuacao;
  Vector2 posicao;
  Orientacao orientacao;
  bool espada;
  Sprite sprites[3];
  enum {
    PARADO = 0,
    CAMINHANDO,
    ATACANDO
  } spriteAtual;
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

void DesenharStatus(int vida, int nivel, int pontuacao) {
  DrawRectangle(0, 0, LARGURA, 60, BLACK);
  DrawText(
	   TextFormat("vida: %d \tnível: %d \tpontuação: %d",
		      vida,
		      nivel,
		      pontuacao),
	   16,
	   16,
	   32,
	   WHITE
	   );
}

int main () {
  InitWindow(LARGURA, ALTURA, "ZINF");

  Texture2D jogadorParado = LoadTexture("assets/jogador_parado.png");
  Texture2D jogadorCaminha = LoadTexture("assets/jogador_caminha.png");
  Texture2D jogadorAtaca = LoadTexture("assets/jogador_ataca.png");

  Jogador jogador = {
    .vida = 3,
    .pontuacao = 0,
    .posicao = (Vector2){ LARGURA / 2, ALTURA / 2},
    .orientacao = LESTE,
    .espada = false,
    .sprites = {
      NovaSprite(&jogadorParado, 4),
      NovaSprite(&jogadorCaminha, 4),
      NovaSprite(&jogadorAtaca, 6),
    },
    .spriteAtual = PARADO
  };

  Jogo jogo = { .jogador = jogador, .nivel = 0 };

  SetTargetFPS(30);

  while(!WindowShouldClose()) {
    AtualizarSprite(&jogador.sprites[jogador.spriteAtual], jogador.orientacao, jogador.posicao, ESCALA);
    
    BeginDrawing();
    ClearBackground(WHITE);
    DesenharSprite(jogador.sprites[jogador.spriteAtual]);
    DesenharStatus(jogador.vida, jogo.nivel, jogador.pontuacao);

    EndDrawing();
  }

  CloseWindow();
}
