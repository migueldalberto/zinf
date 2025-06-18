#ifndef ENTIDADE_H
#define ENTIDADE_H

#include <raylib.h>
#include "sprite.h"
#include "mapa.h"

typedef enum {
  JOGADOR,
  SLIME_VERDE
} EntidadeTipo;

typedef enum {
  DANO = 1,
} Efeitos;

typedef enum { OESTE, LESTE, NORTE, SUL } Orientacao;

typedef struct {
  EntidadeTipo tipo;
  int vida;
  int pontuacao;
  Vector2 posicao;
  Vector2 deslocamento;
  Rectangle hitbox;
  Vector2 hitboxOffset;
  Orientacao orientacao;
  bool espada;
  Sprite sprites[3];
  enum {
    PARADO = 0,
    CAMINHANDO,
    ATACANDO
  } spriteAtual;
  int efeitos;
} Entidade;

bool ChecarColisao(Entidade, Entidade);
void JogadorMove(Entidade*);
void SlimeMove(Entidade*);
void InicializarEntidade(Entidade*, EntidadeTipo, Texture2D*, Vector2);
void DesenharEntidade(Entidade);
Sprite* SpriteAtual(Entidade);
void AtualizarHitbox (Entidade*);
bool ForaDoMapa(Entidade);
void ReverterMovimento(Entidade* e);

#endif
