#include "sprite.h"

#include "defs.h"

void DesenharSprite(Sprite s) {
  DrawTexturePro(*s.textura, s.src, s.dst, (Vector2){ 0.0, 0.0 }, 0.0, WHITE);
}

Sprite NovaSprite(Texture2D* t, int nDeFrames) {
  Sprite s = {
    .textura = t,
    .frameAtual = 0,
    .numeroDeFrames = nDeFrames
  };

  AtualizarSprite(&s, 0, (Vector2){ LARGURA / 2, ALTURA / 2}, ESCALA);

  return s;
}

void AtualizarSprite(Sprite* s, int orientacao,  Vector2 novaPosicao, int escala) {
  int largura = s->textura->width / s->numeroDeFrames;
  int altura = s->textura->height / 4;
  
  s->src = (Rectangle){
    s->frameAtual * largura,
    orientacao * altura,
    largura,
    altura
  };

  s->dst = (Rectangle){
    novaPosicao.x,
    novaPosicao.y,
    largura * escala,
    altura * escala
  };
}
