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
#include "menu.h"

typedef enum {
    MENU_PRINCIPAL,
    JOGANDO,
    FIM_DE_JOGO
} Cena;

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
    m->posicaoMonstros[i].x = (rand() % 24);
    m->posicaoMonstros[i].y = (rand() % 16);
  }
}

void MenuPrincipal(Cena* cenaDoJogo) {
  Botao botaoJogar = NovoBotao("Jogar", LARGURA / 3, ALTURA / 2, DARKBLUE, WHITE);
  Botao botaoRanking = NovoBotao("Ranking", LARGURA / 3, ALTURA / 2 + 100, DARKBLUE, WHITE);
    
  if (IsMouseButtonPressed(0)) {
    Vector2 click = GetMousePosition();
    if (CheckCollisionPointRec(click, botaoJogar.ret)) {
      *cenaDoJogo = JOGANDO;
    } else if (CheckCollisionPointRec(click, botaoRanking.ret)) ;
  }

  BeginDrawing();
  ClearBackground(GRAY);
  DrawText("ZINF", LARGURA / 3, 32, 96, WHITE);
  DesenharBotao(botaoJogar);
  DesenharBotao(botaoRanking);
  EndDrawing();
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
  Texture2D texturaSlimeVerdeMorre = LoadTexture("assets/slime_verde_morre.png");

  Entidade jogador;
  InicializarEntidade(&jogador, JOGADOR, jogadorTexturas, mapa.posicaoJogador);
  int nDeMonstros = mapa.nDeMonstros;
  Entidade monstros[nDeMonstros];
  for (int i = 0; i < mapa.nDeMonstros; ++i) {
    InicializarEntidade(&monstros[i], SLIME_VERDE, &texturaSlimeVerde,
			Vector2Add(Vector2Scale(mapa.posicaoMonstros[i], 32), (Vector2){ 0, ALTURA_STATUS})
			);
  }

  SetTargetFPS(30);

  int contadorDeFrames = 0;
  int velocidadeAnim = 5;
 
  Cena cenaDoJogo = MENU_PRINCIPAL;

  Sprite animacoesDeMorte[10];
  int nDeAnimacoesDeMorte = 0;

  while(!WindowShouldClose()) {
    if (cenaDoJogo == MENU_PRINCIPAL) {
      MenuPrincipal(&cenaDoJogo);
    } else if (cenaDoJogo == FIM_DE_JOGO) {
      BeginDrawing();
      ClearBackground(WHITE);
      DesenharMapa(mapa);
      for (int i = 0; i < mapa.nDeMonstros; ++i)
	DesenharEntidade(monstros[i]);
      DesenharStatus(jogador.vida, nivel, jogador.pontuacao);
      DrawRectangle(0, 0, LARGURA, ALTURA, Fade(BLACK, 0.8));
      DrawText("Você morreu", LARGURA / 4, 96, 96, RED);
      Botao bMenuPrincipal = NovoBotao("Menu Principal", LARGURA / 4, ALTURA / 2 + 100, DARKBLUE, WHITE);
      Botao bSalvarPontuacao = NovoBotao("Salvar pontuação", LARGURA / 4, ALTURA / 2, YELLOW, WHITE);
      DesenharBotao(bMenuPrincipal);
      DesenharBotao(bSalvarPontuacao);
      EndDrawing();
    } else {
      LerControles(&jogador, monstros, mapa.nDeMonstros);
      jogador.posicao = Vector2Add(jogador.posicao, jogador.deslocamento);
      AtualizarHitbox(&jogador);
      AtualizarEfeitos(&jogador);
      if (ForaDoMapa(jogador) || DentroDePedra(jogador, mapa))
	ReverterMovimento(&jogador);

      for (int i = 0; i < mapa.nDeMonstros; ++i) {
	if (monstros[i].vida == 0) {
	  jogador.pontuacao += monstros[i].pontuacao;
	  animacoesDeMorte[nDeAnimacoesDeMorte] = NovaSprite(&texturaSlimeVerdeMorre, 5);
	  AtualizarSprite(&animacoesDeMorte[nDeAnimacoesDeMorte], 0, monstros[i].posicao);
	  ++nDeAnimacoesDeMorte;

	  for (int j = i; j < mapa.nDeMonstros - 1; ++j) {
	    monstros[j] = monstros[j + 1];
	  }
	  mapa.nDeMonstros--;
	}
	
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
      
      if (jogador.vida == 0) {
	cenaDoJogo = FIM_DE_JOGO;
      }

      if (contadorDeFrames >= 30 / velocidadeAnim) {
	contadorDeFrames = 0;

	Sprite *s = &jogador.sprites[jogador.spriteAtual];
	ProximoFrame(s);

	for(int i = 0; i < nDeAnimacoesDeMorte; ++i) {
	  ProximoFrame(&animacoesDeMorte[i]);
	  AtualizarSpriteSrcFrame(&animacoesDeMorte[i]);
	  if (animacoesDeMorte[i].frameAtual == 4) {
	    for (int j = 0; j < nDeAnimacoesDeMorte - 1; ++j) {
	      animacoesDeMorte[j] = animacoesDeMorte[j + 1];
	    }
	    nDeAnimacoesDeMorte--;
	  }
	}
      
	for (int i = 0; i < nDeMonstros; ++i) {
	  Sprite *sm = monstros[i].sprites;
	  ProximoFrame(sm);
	}
      }
    
      AtualizarSprite(&jogador.sprites[jogador.spriteAtual], jogador.orientacao, jogador.posicao);
      for (int i = 0; i < nDeMonstros; ++i)
	AtualizarSprite(monstros[i].sprites, monstros[i].orientacao, monstros[i].posicao);
    
      BeginDrawing();
      ClearBackground(WHITE);
      DesenharMapa(mapa);
      DesenharEntidade(jogador);

      for (int i = 0; i < nDeMonstros; ++i)
	DesenharEntidade(monstros[i]);

      for (int i = 0; i < nDeAnimacoesDeMorte; ++i)
	DesenharSprite(animacoesDeMorte[i]);
    
      DesenharStatus(jogador.vida, nivel, jogador.pontuacao);
      // DesenharHitscan(&jogador);
      EndDrawing();

      ++contadorDeFrames;
    }
  }

  CloseWindow();

  SalvarMapa(mapa, "mapa.txt");
}
