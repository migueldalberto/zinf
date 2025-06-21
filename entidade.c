#include "entidade.h"

#include "defs.h"
#include <raymath.h>

void AtualizarEfeitos(Entidade* e) {
  for (int i = 0; i < 16; ++i) {
    if (e->efeitos & 1<<i) {
      ++e->timerDeEfeitos[i];

      int timeout;
      switch (1<<i) {
      case DANO: timeout = 15; break;
      case ATAQUE: timeout = 15; break;
      }
      
      if(e->timerDeEfeitos[i] >= timeout) {
	e->timerDeEfeitos[i] = 0;
	e->efeitos -= 1<<i;
      }
    }
  }
}

bool ChecarColisao(Entidade a, Entidade b) {
  return CheckCollisionRecs(a.hitbox, b.hitbox);
}

bool ForaDoMapa(Entidade e) {
  return (e.hitbox.width + e.hitbox.x) > LARGURA ||
    (e.hitbox.height + e.hitbox.y) > ALTURA ||
    e.hitbox.x < 0 ||
    e.hitbox.y < ALTURA_STATUS;
}

bool DentroDePedra(Entidade e, Mapa m) {
  for (int i = 0; i < 16; ++i) {
    for (int j = 0; j < 24; ++j) {
      if (m.tiles[i][j] == PEDRA) {
	Rectangle p = (Rectangle){
	  .x = j * 32 * ESCALA,
	  .y = i * 32 * ESCALA + ALTURA_STATUS,
	  32 * ESCALA, 32 * ESCALA
	};
	if (CheckCollisionRecs(p, e.hitbox))
	  return true;
      }
    }
  }
  return false;
}

void DesenharHitbox(Rectangle h) {
  DrawRectangleLinesEx(h, 5.0, RED);
}

void ReverterMovimento(Entidade* e) {
  e->posicao = Vector2Subtract(e->posicao, e->deslocamento);
  AtualizarHitbox(e);
}

void SlimeMove(Entidade* m) {
  if (m->sprites[0].frameAtual == 4) {
    int v = VEL_JOGADOR + 4;
    int dx = 0;
    int dy = 0;

    switch(m->orientacao) {
    case NORTE: dy = v; break;
    case SUL: dy = -v; break;
    case LESTE: dx = v; break;
    case OESTE: dx = -v; break;
    }

    m->deslocamento.x = dx;
    m->deslocamento.y = dy;
  } else {
    m->deslocamento.x = 0;
    m->deslocamento.y = 0;
  }

  m->posicao = Vector2Add(m->posicao, m->deslocamento);
  AtualizarHitbox(m);
}

void AtualizarHitbox (Entidade *e) {
  Vector2 hitboxPos = Vector2Add(e->posicao, e->hitboxOffset);
  e->hitbox.x = hitboxPos.x;
  e->hitbox.y = hitboxPos.y;
}

void InicializarEntidade(Entidade* e, EntidadeTipo t, Texture2D* texturas, Vector2 posicao) {
  switch (t) {
  case JOGADOR:
    *e = (Entidade){
      .tipo = t,
      .vida = 3,
      .pontuacao = 0,
      .posicao = posicao,
      .deslocamento = {0},
      .orientacao = LESTE,
      .espada = false,
      .hitbox = {
	.width = 12 * ESCALA * 2,
	.height = 19 * ESCALA * 2
      },
      .hitboxOffset = {
	.x = 4 * ESCALA,
	.y = 24 * ESCALA
      },
      .sprites = {
	NovaSprite(&texturas[0], 4),
	NovaSprite(&texturas[1], 8),
	NovaSprite(&texturas[2], 3),
      },
      .spriteAtual = PARADO,
      .timerDeEfeitos = {0}
    };
    break;
  case SLIME_VERDE:
    *e = (Entidade){
      .tipo = t,
      .vida = 2,
      .pontuacao = 5,
      .sprites = { NovaSprite(texturas, 5) },
      .posicao = posicao,
      .deslocamento = {0},
      .orientacao = rand() % 4,
      .hitbox = {
	.width = 24 * ESCALA,
	.height = 16 * ESCALA
      },
      .hitboxOffset = {
	4 * ESCALA,
	10 * ESCALA
      },
      .timerDeEfeitos = {0}
    };
    break;
  }
}

void DesenharEntidade(Entidade e) {
  Color tint = (e.efeitos & DANO) ? RED : WHITE;
  switch (e.tipo) {
  case JOGADOR:
    // gambiarra pra sprite de ataque funcionar
    if (e.efeitos & ATAQUE) {
      Sprite* s = &e.sprites[e.spriteAtual];
      int largura = s->textura->width / s->numeroDeFrames;
      int altura = s->textura->height / 4;
      int escala = (32 * ESCALA) / largura ;

      s->dst = (Rectangle){
	e.posicao.x - largura / 2,
	e.posicao.y - altura / 2,
	largura * escala * 3,
	altura * escala * 3
      };
    }
    DesenharSpriteEx(e.sprites[e.spriteAtual], tint);
    break;
  case SLIME_VERDE:
    DesenharSpriteEx(e.sprites[0], tint);
    break;
  }

  //  DesenharHitbox(e.hitbox);
}
