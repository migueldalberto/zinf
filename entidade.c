#include "entidade.h"

#include "defs.h"
#include <raymath.h>
#include <math.h>

void Acelera(Entidade* e, Aceleracao ac) {
  for (int i = 0; i < 5; ++i) {
    if (e->aceleracoes[i].mod == 0) {
      e->aceleracoes[i] = ac;
      break;
    }
  }
}

void AtualizaDeslocamento(Entidade* e) {
  float dy = e->deslocamento.y;
  float dx = e->deslocamento.x;

  for (int i = 0; i < 5; ++i) {
    Aceleracao ac = e->aceleracoes[i];
    dy += sin(ac.ang) * ac.mod;
    dx += cos(ac.ang) * ac.mod;
  }

  e->deslocamento.y = dy;
  e->deslocamento.x = dx;
  while (Vector2Length(e->deslocamento) > e->limiteDeVelocidade) {
    if (abs((int) e->deslocamento.x) > abs((int) e->deslocamento.y)) {
      e->deslocamento.x *= 0.9;
    } else {
      e->deslocamento.y *= 0.9;
    }
  }
}

void MoveEntidade(Entidade* e) {
  AtualizaDeslocamento(e);
  e->posicao = Vector2Add(e->posicao, e->deslocamento);
  AtualizarHitbox(e);
}

void AtualizarEfeitos(Entidade* e) {
  for (int i = 0; i < 16; ++i) {
    if (e->efeitos & 1<<i) {

      int timeout;
      switch (1<<i) {
      case DANO:
	timeout = 15;
	++e->timerDeEfeitos[i];
	if(e->timerDeEfeitos[i] >= timeout) {
	  e->timerDeEfeitos[i] = 0;
	  e->efeitos -= 1<<i;
	}
	break;
      case ATAQUE:
	if (e->sprites[e->spriteAtual].frameAtual == 0 && e->timerDeEfeitos[i]) {
	    e->spriteAtual = PARADO;
	    e->efeitos -= 1<<i;
	    e->timerDeEfeitos[i] = 0;
	} else if (e->sprites[e->spriteAtual].frameAtual == 2)
	  e->timerDeEfeitos[i] = 1;
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

bool ForaDoMapaRet(Rectangle h) {
    return (h.width + h.x) > LARGURA ||
    (h.height + h.y) > ALTURA ||
    h.x < 0 ||
    h.y < ALTURA_STATUS;
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

  if (m->efeitos & DANO) {
    m->deslocamento.x = - m->deslocamento.x;
    m->deslocamento.y = - m->deslocamento.y;
  }

  m->posicao = Vector2Add(m->posicao, m->deslocamento);
  AtualizarHitbox(m);
}

void SlimeOrientacao(Entidade *s, Rectangle hitboxJogador) {
  Rectangle scan = {
    .x = s->hitbox.x - (4 * s->hitbox.width),
    .y = s->hitbox.y - (4 * s->hitbox.height),
    .width = s->hitbox.width * 9,
    .height = s->hitbox.height * 9
  };

  if (CheckCollisionRecs(scan, hitboxJogador)) {
    float dy = hitboxJogador.y - s->hitbox.y;
    float dx = hitboxJogador.x - s->hitbox.x;

    if (abs((int) dy) > abs((int) dx)) {
      s->orientacao = dy > 0 ? NORTE : SUL;
    } else {
      s->orientacao = dx > 0 ? LESTE : OESTE;
    }
  } else if (ForaDoMapaRet(scan)) {
    float dcima = s->hitbox.y;
    float dbaixo = ALTURA - s->hitbox.y;
    float desq = s->hitbox.x;
    float ddir = LARGURA - s->hitbox.x;

    if (dcima < dbaixo && dcima < desq && dcima < ddir)
      s->orientacao = NORTE;
    else if (dbaixo < dcima && dbaixo < desq && dbaixo < ddir)
      s->orientacao = SUL;
    else if (desq < dcima && desq < dbaixo && desq < ddir)
      s->orientacao = LESTE;
    else
      s->orientacao = OESTE;
  }
}

void AtualizarHitbox (Entidade *e) {
  Vector2 hitboxPos = Vector2Add(e->posicao, e->hitboxOffset);
  e->hitbox.x = hitboxPos.x;
  e->hitbox.y = hitboxPos.y;
}

void InicializarEntidade(Entidade* e, EntidadeTipo t, Texture2D* texturas, Vector2 posicao) {
  *e = (Entidade){0};
  
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
	.width = 13 * ESCALA * 2,
	.height = 19 * ESCALA * 2
      },
      .hitboxOffset = {
	.x = 2 * ESCALA * 2,
	.y = 12 * ESCALA * 2
      },
      .sprites = {
	NovaSprite(&texturas[0], 4, true),
	NovaSprite(&texturas[1], 8, true),
	NovaSprite(&texturas[2], 3, true),
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
      .sprites = { NovaSprite(texturas, 5, true) },
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
  case SLIME_VERDE_MORTE:
    *e = (Entidade) {
      .tipo = t,
      .sprites = { NovaSprite(texturas, 5, true) },
      .posicao = posicao
    };
    break;
  case VIDA_EXTRA:
    *e = (Entidade) {
      .tipo = t,
      .posicao = posicao,
      .sprites = { NovaSprite(texturas, 4, false) },
      .hitbox = { posicao.x, posicao.y }
    };
    e->hitbox.width = LarguraDaSprite(e->sprites[0]) * ESCALA * 2;
    e->hitbox.height = e->sprites[0].textura->height * ESCALA * 2;
    break;
  case ESPADA:
    *e = (Entidade) {
      .tipo = t,
      .posicao = posicao,
      .sprites = { NovaSprite(texturas, 1, false) },
      .hitbox = { posicao.x, posicao.y }
    };
    e->hitbox.width = LarguraDaSprite(e->sprites[0]) * ESCALA * 2;
    e->hitbox.height = e->sprites[0].textura->height * ESCALA * 2;
    break;
  case PEDRA:
    *e = (Entidade) {
      .tipo = t,
      .posicao = posicao,
      .sprites = { NovaSprite(texturas, 1, false) },
      .hitbox = { posicao.x, posicao.y },
      .obstaculo = true
    };
    e->hitbox.width = (LarguraDaSprite(e->sprites[0]) - 2) * ESCALA;
    e->hitbox.height = ( e->sprites[0].textura->height - 10) * ESCALA;
    e->hitboxOffset.x = 1;
    e->hitboxOffset.y = 0;
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

      int offsetX = (largura - ((e.sprites[PARADO].textura->width / e.sprites[PARADO].numeroDeFrames)) / 2) - 3;

      s->dst = (Rectangle){
	e.posicao.x - (offsetX * ESCALA),
	e.posicao.y - (5 * ESCALA),
	largura * escala * 3,
	altura * escala * 3
      };
    } 
    
    DesenharSpriteEx(e.sprites[e.spriteAtual], tint);
    break;
  default:
    DesenharSpriteEx(e.sprites[0], tint);
    break;
  }

   /* DesenharHitbox(e.hitbox); */
}

void AtualizarSprite(Entidade* e) {
  Sprite* s = &e->sprites[e->spriteAtual];
  
  s->src.width = (s->textura->width / s->numeroDeFrames);
  s->src.height = s->usarOrientacao ? s->textura->height / 4 : s->textura->height;
  s->src.x = s->frameAtual * s->src.width;
  s->src.y = e->orientacao * s->src.height;

  s->dst.x = e->posicao.x;
  s->dst.y = e->posicao.y;
  s->dst.width = s->src.width * 32 * ESCALA / s->src.width;
  s->dst.height = s->src.height * ESCALA * 32 / s->src.width;
}
