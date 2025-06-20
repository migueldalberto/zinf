#include  "controles.h"

#include "defs.h"

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

  if (IsKeyPressed(KEY_T) && !(jogador->efeitos & ATAQUE)) {
    jogador->efeitos = ATAQUE;
    jogador->spriteAtual = ATACANDO;
    Rectangle hitscan = {
      jogador->hitbox.x, jogador->hitbox.y,
      jogador->hitbox.height, jogador->hitbox.height
    };

    switch (jogador->orientacao) {
    case NORTE:
      hitscan.y -= jogador->hitbox.height;
      break;
    case SUL:
      hitscan.y += jogador->hitbox.height;
      break;
    case LESTE: hitscan.x += jogador->hitbox.width;
      break;
    case OESTE: hitscan.x -= jogador->hitbox.width;
      break;
    }

    for (int i = 0; i < nDeMonstros; ++i) {
      if (CheckCollisionRecs(monstros[i].hitbox, hitscan)) {
	monstros[i].efeitos |= DANO;
	--monstros[i].vida;
      }
    }
  }
}

