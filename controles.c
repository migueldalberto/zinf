#include  "controles.h"

#include "defs.h"

Rectangle GerarHitscan(Entidade* jogador) {
  Rectangle hitscan = {
    jogador->hitbox.x, jogador->hitbox.y,
    jogador->hitbox.height * 2, jogador->hitbox.height * 2,
  };

  switch (jogador->orientacao) {
  case NORTE:
    hitscan.height /= 2;
    hitscan.y = jogador->hitbox.y - hitscan.height;
    hitscan.x = jogador->hitbox.x - (hitscan.width - jogador->hitbox.width) / 2;
    break;
  case SUL:
    hitscan.height /= 2;
    hitscan.y = jogador->hitbox.y + jogador->hitbox.height;
    hitscan.x = jogador->hitbox.x - (hitscan.width - jogador->hitbox.width) / 2;
    break;
  case LESTE:
    hitscan.width /= 2;
    hitscan.x = jogador->hitbox.x + jogador->hitbox.width;
    hitscan.y = jogador->hitbox.y - (hitscan.height - jogador->hitbox.height) / 2;
    break;
  case OESTE:
    hitscan.width /= 2;
    hitscan.x = jogador->hitbox.x - hitscan.width;
    hitscan.y = jogador->hitbox.y - (hitscan.height - jogador->hitbox.height) / 2;
    break;
  }

  return hitscan;
}

void LerControles (Entidade* jogador, Entidade* monstros, int nDeMonstros) {
  bool n,s,l,o;
  n = IsKeyDown(KEY_W);
  s = IsKeyDown(KEY_S);
  l = IsKeyDown(KEY_D) && ! (n || s);
  o = IsKeyDown(KEY_A) && ! (n || s);

  if (jogador->efeitos & ATAQUE) {
    jogador->deslocamento = (Vector2){ 0, 0 };
    jogador->spriteAtual = ATACANDO;
  } else {
    jogador->deslocamento = (Vector2){
      (n || s) ? 0 : (l - o) * VEL_JOGADOR,
      (l || o) ? 0 : (s - n) * VEL_JOGADOR
    };

    jogador->spriteAtual = (jogador->deslocamento.x != 0 || jogador->deslocamento.y != 0) ? CAMINHANDO : PARADO;

    if (n && !(l || o))
      jogador->orientacao = NORTE;
    else if (s && !(l || o))
      jogador->orientacao = SUL;
    else if (l && !(n || s))
      jogador->orientacao = LESTE;
    else if (o && !(n || s))
      jogador->orientacao = OESTE;
  }  

  if (IsKeyPressed(KEY_T) && !(jogador->efeitos & ATAQUE) && jogador->espada) {
    jogador->efeitos = ATAQUE;
    jogador->spriteAtual = ATACANDO;
    Rectangle hitscan = GerarHitscan(jogador);

    for (int i = 0; i < nDeMonstros; ++i) {
      if (monstros[i].tipo == SLIME_VERDE && CheckCollisionRecs(monstros[i].hitbox, hitscan)) {
	monstros[i].efeitos |= DANO;
	--monstros[i].vida;
      }
    }
  }
}

void DesenharHitscan(Entidade* jogador) {
  DrawRectangleRec(GerarHitscan(jogador), Fade(RED, 0.5));
}
