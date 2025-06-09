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

Sprite NovaSprite(Texture2D*, int nDeFrames);
void DesenharSprite(Sprite);
void AtualizarSprite(Sprite*, int orientacao, Vector2 novaPosicao);

#endif
