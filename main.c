#include <raylib.h>
#include <raymath.h>

#include <stdbool.h>
#include <time.h>
#include <stdlib.h>

#include "defs.h"
#include "sprite.h"
#include "mapa.h"
#include "entidade.h"
#include "controles.h"

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

  int i = 4;
  for (int j = 8; j < 18; ++j) 
    m->tiles[i][j] = PEDRA;

   i = 12;
  for (int j = 8; j < 18; ++j) 
    m->tiles[i][j] = PEDRA;

  MapaGerarRetangulos(m);

  m->textura = LoadTexture("assets/tileset.png");
  m->posicaoJogador = (Vector2){ (8 * 32), 6 * 32};

  m->nDeMonstros = (rand() % 10) + 1;

  for (int i = 0; i < m->nDeMonstros; ++i) {
    m->posicaoMonstros[i].x = (rand() % 24) * 32;
    m->posicaoMonstros[i].y = ((rand() % 16) * 32) + ALTURA_STATUS;
  }
}

int main () {
  int nivel = 0;
  
  InitWindow(LARGURA, ALTURA, "ZINF");

  srand(time(NULL));

  Mapa mapa;

  IniciarMapaTeste(&mapa);

  Texture2D jogadorTexturas[3] = {
    LoadTexture("assets/jogador_parado.png"),
    LoadTexture("assets/jogador_caminha.png"),
    LoadTexture("assets/jogador_ataca.png")
  };

  Texture2D texturaSlimeVerde = LoadTexture("assets/slime_verde_anda.png");

  Entidade jogador;
  InicializarEntidade(&jogador, JOGADOR, jogadorTexturas, mapa.posicaoJogador);

  Entidade monstros[mapa.nDeMonstros];
  for (int i = 0; i < mapa.nDeMonstros; ++i)
    InicializarEntidade(&monstros[i], SLIME_VERDE, &texturaSlimeVerde, mapa.posicaoMonstros[i]);

  SetTargetFPS(30);

  int contadorDeFrames = 0;
  int velocidadeAnim = 5;

  while(!WindowShouldClose()) {
    LerControles(&jogador, monstros, mapa.nDeMonstros);
    jogador.posicao = Vector2Add(jogador.posicao, jogador.deslocamento);
    AtualizarHitbox(&jogador);
    AtualizarEfeitos(&jogador);
    if (ForaDoMapa(jogador) || DentroDePedra(jogador, mapa))
      ReverterMovimento(&jogador);

    for (int i = 0; i < mapa.nDeMonstros; ++i) {
      SlimeOrientacao(&monstros[i], jogador.hitbox);
      SlimeMove(&monstros[i]);
      if (ForaDoMapa(monstros[i]) || DentroDePedra(monstros[i], mapa))
	ReverterMovimento(&monstros[i]);

      AtualizarEfeitos(&monstros[i]);

      if (ChecarColisao(monstros[i], jogador) && !(jogador.efeitos & DANO)) {
	jogador.efeitos |= DANO;
	jogador.posicao = Vector2Add(jogador.posicao, Vector2Scale(monstros[i].deslocamento, 2));
	jogador.vida -= 1;
	AtualizarHitbox(&jogador);
      }
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
    for (int i = 0; i < mapa.nDeMonstros; ++i)
      AtualizarSprite(monstros[i].sprites, monstros[i].orientacao, monstros[i].posicao);
    
    BeginDrawing();
    ClearBackground(WHITE);
    DesenharMapa(mapa);
    DesenharEntidade(jogador);
    for (int i = 0; i < mapa.nDeMonstros; ++i)
      DesenharEntidade(monstros[i]);
    
    DesenharStatus(jogador.vida, nivel, jogador.pontuacao);
    EndDrawing();

    ++contadorDeFrames;
  }

  CloseWindow();
}
