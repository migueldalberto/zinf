#include "entidade.h"

#include "defs.h"
#include <raymath.h>

bool ChecarColisao(Entidade a, Entidade b) {
  return CheckCollisionRecs(a.hitbox, b.hitbox);
}

bool ForaDoMapa(Entidade e) {
  return (e.hitbox.width + e.hitbox.x) > LARGURA ||
    (e.hitbox.height + e.hitbox.y) > ALTURA ||
    e.hitbox.x < 0 ||
    e.hitbox.y < ALTURA_STATUS;
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
	NovaSprite(&texturas[2], 6),
      },
      .spriteAtual = PARADO
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
      }
    };
    break;
  }
}

void DesenharEntidade(Entidade e) {
  Color tint = (e.efeitos & DANO) ? RED : WHITE;
  switch (e.tipo) {
  case JOGADOR:
    DesenharSpriteEx(e.sprites[e.spriteAtual], tint);
    break;
  case SLIME_VERDE:
    DesenharSpriteEx(e.sprites[0], tint);
    break;
  }

  //  DesenharHitbox(e.hitbox);
}
