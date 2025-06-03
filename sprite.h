#ifndef SPRITE_H
#define SPRITE_H

#include <raylib.h>

typedef struct {
  Texture2D* textura;
  Rectangle src;
  Rectangle dst;
  int frameAtual;
  int numeroDeFrames;
} Sprite;

void DesenharSprite(Sprite);
Sprite NovaSprite(Texture2D*, int nDeFrames);
void AtualizarSprite(Sprite*, int orientacao,  Vector2 novaPosicao, int escala);

#endif
