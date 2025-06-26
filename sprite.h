#ifndef SPRITE_H
#define SPRITE_H

#include <raylib.h>

typedef struct {
  Texture2D* textura;
  Rectangle src;
  Rectangle dst;
  int frameAtual;
  int numeroDeFrames;
  bool usarOrientacao;
} Sprite;

Sprite NovaSprite(Texture2D*, int nDeFrames, bool);
float LarguraDaSprite(Sprite);
void DesenharSprite(Sprite);
void DesenharSpriteEx(Sprite, Color tint);
void ProximoFrame(Sprite*);
void AtualizarSpriteSrcFrame(Sprite*);

#endif
