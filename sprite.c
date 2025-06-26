#include "sprite.h"

#include "defs.h"

#include <assert.h>

float LarguraDaSprite (Sprite s) {
  return s.textura->width / s.numeroDeFrames;
}

void DesenharSprite(Sprite s) {
  DrawTexturePro(*s.textura, s.src, s.dst, (Vector2){ 0.0, 0.0 }, 0.0, WHITE);
}

void DesenharSpriteEx(Sprite s, Color tint) {
  DrawTexturePro(*s.textura, s.src, s.dst, (Vector2){ 0.0, 0.0 }, 0.0, tint);
}

Sprite NovaSprite(Texture2D* t, int nDeFrames, bool usarOrientacao) {
  Sprite s = {
    .textura = t,
    .frameAtual = 0,
    .numeroDeFrames = nDeFrames,
    .usarOrientacao = usarOrientacao
  };

  return s;
}

/* void AtualizarSprite(Sprite* s, int orientacao, Vector2 novaPosicao) { */
/*   assert(s->numeroDeFrames != 0); */
/*   assert(s->textura->width != 0); */
/*   int largura = s->textura->width / s->numeroDeFrames; */
/*   int altura = s->usarOrientacao ? s->textura->height / 4 : s->textura->height; */
/*   int escala = (32 * ESCALA) / largura; */
  
/*   s->src = (Rectangle){ */
/*     s->frameAtual * largura, */
/*     orientacao * altura, */
/*     largura, */
/*     altura */
/*   }; */

/*   s->dst = (Rectangle){ */
/*     novaPosicao.x, */
/*     novaPosicao.y, */
/*     largura * escala, */
/*     altura * escala */
/*   }; */
/* } */

void AtualizarSpriteSrcFrame(Sprite* s) {
  s->src.x = s->frameAtual * (s->textura->width / s->numeroDeFrames);
}

void ProximoFrame(Sprite* s) {
  s->frameAtual++;
  if (s->frameAtual >= s->numeroDeFrames)
    s->frameAtual = 0;
}
