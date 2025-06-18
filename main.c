#include <raylib.h>
#include <raymath.h>

#include <stdbool.h>
#include <time.h>
#include <stdlib.h>

#include "defs.h"
#include "sprite.h"
#include "mapa.h"
#include "entidade.h"

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

void LerControles (Entidade* jogador) {
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
    LerControles(&jogador);
    jogador.posicao = Vector2Add(jogador.posicao, jogador.deslocamento);
    AtualizarHitbox(&jogador);
    if (ForaDoMapa(jogador))
      ReverterMovimento(&jogador);

    for (int i = 0; i < mapa.nDeMonstros; ++i) {
      SlimeMove(&monstros[i]);
      if (ForaDoMapa(monstros[i]))
	ReverterMovimento(&monstros[i]);

      if (ChecarColisao(monstros[i], jogador)) {
	//	jogador.efeitos |= DANO;
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
