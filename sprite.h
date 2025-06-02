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

#endif
