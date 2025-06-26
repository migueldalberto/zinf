#ifndef ENTIDADE_H
#define ENTIDADE_H

#include <raylib.h>
#include "sprite.h"
#include "mapa.h"

typedef enum {
  DANO = 1,
  ATAQUE = 1<<1,
} Efeitos;

typedef struct {
  float ang;
  float mod;
  int timeout;
  int frames;
} Aceleracao;

typedef enum { OESTE, LESTE, NORTE, SUL } Orientacao;

typedef struct {
  EntidadeTipo tipo;
  int vida;
  int pontuacao;
  Vector2 posicao;
  Vector2 deslocamento;
  Aceleracao aceleracoes[5];
  float limiteDeVelocidade;
  Rectangle hitbox;
  Vector2 hitboxOffset;
  Orientacao orientacao;
  bool espada;
  bool obstaculo;
  Sprite sprites[3];
  enum {
    PARADO = 0,
    CAMINHANDO,
    ATACANDO
  } spriteAtual;
  int efeitos;
  int timerDeEfeitos[16];
} Entidade;

void AtualizarEfeitos(Entidade*);
bool ChecarColisao(Entidade, Entidade);
void JogadorMove(Entidade*);
void SlimeMove(Entidade*);
void InicializarEntidade(Entidade*, EntidadeTipo, Texture2D*, Vector2);
void DesenharEntidade(Entidade);
Sprite* SpriteAtual(Entidade);
void AtualizarHitbox (Entidade*);
bool ForaDoMapa(Entidade);
void ReverterMovimento(Entidade* e);
void SlimeOrientacao(Entidade *, Rectangle hitboxJogador);
void AtualizarSprite(Entidade*);

#endif
