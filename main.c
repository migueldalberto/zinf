#include <raylib.h>
#include <raymath.h>

#include <stdbool.h>
#include <time.h>
#include <stdlib.h>

#include "defs.h"
#include "sprite.h"
#include "mapa.h"

typedef enum { OESTE, LESTE, NORTE, SUL } Orientacao;

typedef struct {
  int vida;
  int pontuacao;
  Vector2 posicao;
  Vector2 deslocamento;
  Rectangle hitbox;
  Orientacao orientacao;
  bool espada;
  Sprite sprites[3];
  enum {
    PARADO = 0,
    CAMINHANDO,
    ATACANDO
  } spriteAtual;
} Jogador;

typedef struct {
  Sprite sprite;
  int vida;
  int pontuacao;
  Vector2 posicao;
  Vector2 deslocamento;
  Orientacao orientacao;
} Monstro;

void DesenharStatus(int vida, int nivel, int pontuacao) {
  DrawRectangle(0, 0, LARGURA, ALTURA_STATUS, BLACK);
  DrawText(
	   TextFormat("vida: %d \tnível: %d \tpontuação: %d",
		      vida,
		      nivel,
		      pontuacao),
	   16,
	   16,
	   32,
	   WHITE
	   );
}

void IniciarMapaTeste(Mapa* m) {
  for (int i = 0; i < 16; ++i)
    for (int j = 0; j < 24; ++j)
      m->tiles[i][j] = GRAMA;


  MapaGerarRetangulos(m);

  m->textura = LoadTexture("assets/tileset.png");
  m->posicaoJogador = (Vector2){ (8 * 32), 6 * 32};

  m->nDeMonstros = (rand() % 10) + 1;

  for (int i = 0; i < m->nDeMonstros; ++i) {
    m->posicaoMonstros[i].x = (rand() % 24) * 32;
    m->posicaoMonstros[i].y = (rand() % 16) * 32;
  }
}

void MonstroMove(Monstro* m) {
  if (m->sprite.frameAtual == 4) {
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
}

void LerControles (Jogador* jogador) {
  bool n,s,l,o;
  n = IsKeyDown(KEY_W);
  s = IsKeyDown(KEY_S);
  l = IsKeyDown(KEY_D) && ! (n || s);
  o = IsKeyDown(KEY_A) && ! (n || s);

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

int main () {
  int nivel = 0;
  
  InitWindow(LARGURA, ALTURA, "ZINF");

  srand(time(NULL));

  Mapa mapa;

  IniciarMapaTeste(&mapa);

  Texture2D jogadorParado = LoadTexture("assets/jogador_parado.png");
  Texture2D jogadorCaminha = LoadTexture("assets/jogador_caminha.png");
  Texture2D jogadorAtaca = LoadTexture("assets/jogador_ataca.png");

  Texture2D texturaMonstro = LoadTexture("assets/slime_verde_anda.png");

  Jogador jogador = {
    .vida = 3,
    .pontuacao = 0,
    .posicao = mapa.posicaoJogador,
    .deslocamento = {0},
    .orientacao = LESTE,
    .espada = false,
    .hitbox = {
      .x = mapa.posicaoJogador.x,
      .y = mapa.posicaoJogador.y + 12,
      .width = 12,
      .height = 20
    },
    .sprites = {
      NovaSprite(&jogadorParado, 4),
      NovaSprite(&jogadorCaminha, 8),
      NovaSprite(&jogadorAtaca, 6),
    },
    .spriteAtual = PARADO
  };

  Monstro monstros[mapa.nDeMonstros];

  for (int i = 0; i < mapa.nDeMonstros; ++i) {
    monstros[i] = (Monstro){
      .vida = 2,
      .pontuacao = 5,
      .sprite = NovaSprite(&texturaMonstro, 5),
      .posicao = mapa.posicaoMonstros[i],
      .deslocamento = {0},
      .orientacao = rand() % 4
    };
  }
  
  SetTargetFPS(30);

  int contadorDeFrames = 0;
  int velocidadeAnim = 5;

  while(!WindowShouldClose()) {
    LerControles(&jogador);
    jogador.posicao = Vector2Add(jogador.posicao, jogador.deslocamento);

    for (int i = 0; i < mapa.nDeMonstros; ++i) {
      MonstroMove(&monstros[i]);
    }
    
    if (contadorDeFrames >= 30 / velocidadeAnim) {
      contadorDeFrames = 0;

      Sprite *s = &jogador.sprites[jogador.spriteAtual];
      ProximoFrame(s);
      
      for (int i = 0; i < mapa.nDeMonstros; ++i) {
	Sprite *sm = monstros[i].sprites;
	ProximoFrame(sm);
      }
    }
    
    AtualizarSprite(&jogador.sprites[jogador.spriteAtual], jogador.orientacao, jogador.posicao);
    for (int i = 0; i < mapa.nDeMonstros; ++i) {
      AtualizarSprite(&monstros[i].sprite, monstros[i].orientacao, monstros[i].posicao);
    }
    
    BeginDrawing();
    ClearBackground(WHITE);
    DesenharMapa(mapa);
    DesenharSprite(jogador.sprites[jogador.spriteAtual]);
    for (int i = 0; i < mapa.nDeMonstros; ++i) {
      DesenharSprite(monstros[i].sprite);
    }
    
    DesenharStatus(jogador.vida, nivel, jogador.pontuacao);

    EndDrawing();

    ++contadorDeFrames;
  }

  CloseWindow();
}
