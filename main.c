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
#include "ranking.h"
#include <string.h>

typedef enum {
    MENU_PRINCIPAL,
    JOGANDO,
    FIM_DE_JOGO,
    LER_NOME,
    RANKING
} Cena;

char* niveis[3] = {
  "mapas/mapa01.txt",
  "mapas/mapa02.txt",
  NULL,
};

typedef struct {
  Texture2D tileset;
  Texture2D jogador[3];
  Texture2D slime;
  Texture2D slimeMorre;
} Texturas;

typedef struct {
  Ranking ranking;
  Entidade jogador;
  Entidade monstros[10];
  int nDeMonstros;
  int nivel;
  Cena cena;
  Mapa mapa;
  bool gerarNiveisAleatorios;
  Texturas* texturas;
} Jogo;

void InicioDeJogo(Jogo* jg) {
  if (CarregarMapa(&jg->mapa, niveis[jg->nivel]))
    jg->gerarNiveisAleatorios = true;

  InicializarEntidade(&jg->jogador,
		      JOGADOR,
		      jg->texturas->jogador,
		      ConverterPosicaoMapaTela(jg->mapa.posicaoJogador));
  jg->nDeMonstros = jg->mapa.nDeMonstros;
  for (int i = 0; i < jg->mapa.nDeMonstros; ++i) {
    InicializarEntidade(&jg->monstros[i], SLIME_VERDE, &jg->texturas->slime,
			ConverterPosicaoMapaTela(jg->mapa.posicaoMonstros[i])
			);
  }
}

void TrocarCena(Jogo* j, Cena dst) {
  switch (dst) {
  case RANKING:
    CarregarRanking(&j->ranking, ARQ_RANKING);
    break;
  case JOGANDO:
    InicioDeJogo(j);
    break;
  case MENU_PRINCIPAL: break;
  case FIM_DE_JOGO: break;
  case LER_NOME: break;
  }
  
  j->cena = dst;
}

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

void CenaRanking(Jogo* j) {
  Botao bMenuPrincipal = NovoBotao("<=", 16, 16, RED, WHITE);

  if (CliqueNoBotao(bMenuPrincipal)) {
    TrocarCena(j, MENU_PRINCIPAL);
  }
  
  BeginDrawing();
  ClearBackground(GRAY);

  int cw = 16;
  DrawText("Ranking", LARGURA / 2 - (3.5 * cw), 16, 32, WHITE);
  DesenharBotao(bMenuPrincipal);
  
  if (j->ranking.tam != 0) {
    for (int i = 0; i < j->ranking.tam; ++i) {
      Color c = i % 2 == 0 ? YELLOW : ORANGE;
      DrawText(j->ranking.linhas[i].nome, LARGURA / 2 - (20 * cw), 16 + (i * 36), 32, c);
      DrawText(TextFormat("%9d", j->ranking.linhas[i].score), LARGURA / 2 + (9 * cw), 16 + (i * 36), 32, c);
    }
  } else
    DrawText("--ranking vazio--", LARGURA / 2 - (8.5 * cw), 20 + 32, 32, YELLOW);
  
  EndDrawing();
}

void MenuPrincipal(Jogo* j) {
  Botao botaoJogar = NovoBotao("Jogar", LARGURA / 2 - 16 * 2.5, ALTURA / 2, DARKBLUE, WHITE);
  Botao botaoRanking = NovoBotao("Ranking", LARGURA / 2 - 16 * 3.5, ALTURA / 2 + 100, DARKBLUE, WHITE);

  if (CliqueNoBotao(botaoJogar))
    TrocarCena(j, JOGANDO);
  else if (CliqueNoBotao(botaoRanking))
    TrocarCena(j, RANKING);

  BeginDrawing();
  ClearBackground(GRAY);
  DrawText("ZINF", LARGURA / 2 - (16 * 2), 32, 96, WHITE);
  DesenharBotao(botaoJogar);
  DesenharBotao(botaoRanking);
  EndDrawing();
}

void
LerNome(char* buf, int cap, int* tam, Jogo* j) {
  Rectangle textBox = { LARGURA / 2 - (cap / 2 * 32), ALTURA / 2, 32 * cap + 16, 36};

  int key = GetCharPressed();

  while (key > 0) {
    if (key >= 32 && key <= 125 && *tam < cap) {
      buf[*tam] = (char) key;
      buf[*tam + 1] = '\0';
      *tam += 1;
    }

    key = GetCharPressed();
  }

  if (IsKeyPressed(KEY_BACKSPACE)){
    *tam -= 1;
    if (*tam < 0) *tam = 0;
    buf[*tam] = '\0';
  }

  if (IsKeyPressed(KEY_ENTER)) {
    Ranking r;
    if (CarregarRanking(&r, "ranking.bin") == 1) {
      InicializarRanking(&r);
    }

    RankingLinha rl;
    strcpy(rl.nome, buf);
    rl.score = j->jogador.pontuacao;

    RankingAdd(&r, rl);
    SalvarRanking(r, "ranking.bin");
    FreeRanking(r);
    
    TrocarCena(j, RANKING);
  }
  
  BeginDrawing();
  ClearBackground(GRAY);
  DrawRectangleRec(textBox, LIGHTGRAY);
  DrawText(buf, textBox.x + 4, textBox.y + 8, 32, BLACK);
  EndDrawing();
}

void FimDeJogo (Jogo* j, Texture2D* tileset) {
  Botao bMenuPrincipal = NovoBotao("Menu Principal", LARGURA / 2 - (16 * 7), ALTURA / 2 + 100, DARKBLUE, WHITE);
  Botao bSalvarPontuacao = NovoBotao("Salvar pontuação", LARGURA / 2 - (16 * 8), ALTURA / 2, DARKBLUE, WHITE);

  if (CliqueNoBotao(bMenuPrincipal))
    TrocarCena(j, MENU_PRINCIPAL);

  if (CliqueNoBotao(bSalvarPontuacao))
    TrocarCena(j, LER_NOME);
  
  BeginDrawing();
  ClearBackground(WHITE);
  DesenharMapa(j->mapa, tileset);
  for (int i = 0; i < j->nDeMonstros; ++i)
    DesenharEntidade(j->monstros[i]);
  
  DesenharStatus(j->jogador.vida, j->nivel, j->jogador.pontuacao);
  DrawRectangle(0, 0, LARGURA, ALTURA, Fade(BLACK, 0.8));
  DrawText("Você morreu", LARGURA / 2 - (48 * 5.5), 96, 96, RED);
  DrawText(TextFormat("Sua pontuação: %06d", j->jogador.pontuacao), LARGURA / 2 - (32 * 10.5), 192, 64, YELLOW);
  DesenharBotao(bMenuPrincipal);
  DesenharBotao(bSalvarPontuacao);
  EndDrawing();
}

int main () {
  InitWindow(LARGURA, ALTURA, "ZINF");
  srand(time(NULL));

  Texturas texturas = {
    .tileset = LoadTexture("assets/tileset.png"),
    .jogador = {
      LoadTexture("assets/jogador_parado.png"),
      LoadTexture("assets/jogador_caminha.png"),
      LoadTexture("assets/jogador_ataca.png")
    },
    .slime = LoadTexture("assets/slime_verde_anda.png"),
    .slimeMorre = LoadTexture("assets/slime_verde_morre.png")
  };
  
  Jogo jg;
  jg.texturas = &texturas;
  
  // IniciarMapaTeste(&j.mapa);
  jg.cena = MENU_PRINCIPAL;

  SetTargetFPS(30);

  int contadorDeFrames = 0;
  int velocidadeAnim = 5;

  Sprite animacoesDeMorte[10];
  int nDeAnimacoesDeMorte = 0;

  char cbuf[21];
  int tamBuf = 0;

  while(!WindowShouldClose()) {
    if (jg.cena == MENU_PRINCIPAL)
      MenuPrincipal(&jg);
    else if (jg.cena == FIM_DE_JOGO)
      FimDeJogo(&jg, &texturas.tileset);
    else if (jg.cena == RANKING)
      CenaRanking(&jg);
    else if (jg.cena == LER_NOME) {
      LerNome(cbuf, 20, &tamBuf, &jg);
    } else {
      LerControles(&jg.jogador, jg.monstros, jg.nDeMonstros);
      jg.jogador.posicao = Vector2Add(jg.jogador.posicao, jg.jogador.deslocamento);
      AtualizarHitbox(&jg.jogador);
      AtualizarEfeitos(&jg.jogador);
      if (ForaDoMapa(jg.jogador) || DentroDePedra(jg.jogador, jg.mapa))
	ReverterMovimento(&jg.jogador);

      for (int i = 0; i < jg.nDeMonstros; ++i) {
	if (jg.monstros[i].vida == 0) {
	  jg.jogador.pontuacao += jg.monstros[i].pontuacao;
	  animacoesDeMorte[nDeAnimacoesDeMorte] = NovaSprite(&texturas.slimeMorre, 5);
	  AtualizarSprite(&animacoesDeMorte[nDeAnimacoesDeMorte], 0, jg.monstros[i].posicao);
	  ++nDeAnimacoesDeMorte;

	  for (int j = i; j < jg.nDeMonstros - 1; ++j) {
	    jg.monstros[j] = jg.monstros[j + 1];
	  }
	  jg.nDeMonstros--;
	}
	
	SlimeOrientacao(&jg.monstros[i], jg.jogador.hitbox);
	SlimeMove(&jg.monstros[i]);
	if (ForaDoMapa(jg.monstros[i]) || DentroDePedra(jg.monstros[i], jg.mapa))
	  ReverterMovimento(&jg.monstros[i]);

	AtualizarEfeitos(&jg.monstros[i]);

	if (ChecarColisao(jg.monstros[i], jg.jogador) && !(jg.jogador.efeitos & DANO)) {
	  jg.jogador.efeitos |= DANO;
	  jg.jogador.posicao = Vector2Add(jg.jogador.posicao, Vector2Scale(jg.monstros[i].deslocamento, 2));
	  jg.jogador.vida -= 1;
	  AtualizarHitbox(&jg.jogador);
	}
      }

      if (jg.nDeMonstros == 0) {
	++jg.nivel;
	if (jg.gerarNiveisAleatorios) {
	  GerarMapaAleatorio(&jg.mapa, jg.nivel);
	} else if (niveis[jg.nivel] == NULL)
	  jg.gerarNiveisAleatorios = true;
	else
	  CarregarMapa(&jg.mapa, niveis[jg.nivel]);
	
	jg.nDeMonstros = jg.mapa.nDeMonstros;
	for (int i = 0; i < jg.mapa.nDeMonstros; ++i) {
	  jg.jogador.espada = false;
	  jg.jogador.posicao = ConverterPosicaoMapaTela(jg.mapa.posicaoJogador);
	  InicializarEntidade(&jg.monstros[i], SLIME_VERDE, &texturas.slime,
			      ConverterPosicaoMapaTela(jg.mapa.posicaoMonstros[i]));
	}
      }
      
      if (jg.jogador.vida == 0) {
	TrocarCena(&jg, FIM_DE_JOGO);
      }

      if (contadorDeFrames >= 30 / velocidadeAnim) {
	contadorDeFrames = 0;

	Sprite *s = &jg.jogador.sprites[jg.jogador.spriteAtual];
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
      
	for (int i = 0; i < jg.nDeMonstros; ++i) {
	  Sprite *sm = jg.monstros[i].sprites;
	  ProximoFrame(sm);
	}
      }
    
      for (int i = 0; i < jg.nDeMonstros; ++i)
	AtualizarSprite(jg.monstros[i].sprites, jg.monstros[i].orientacao, jg.monstros[i].posicao);
      AtualizarSprite(&jg.jogador.sprites[jg.jogador.spriteAtual], jg.jogador.orientacao, jg.jogador.posicao);

    
      BeginDrawing();
      ClearBackground(WHITE);
      DesenharMapa(jg.mapa, &texturas.tileset);
      DesenharEntidade(jg.jogador);

      for (int i = 0; i < jg.nDeMonstros; ++i)
	DesenharEntidade(jg.monstros[i]);

      for (int i = 0; i < nDeAnimacoesDeMorte; ++i)
	DesenharSprite(animacoesDeMorte[i]);
    
      DesenharStatus(jg.jogador.vida, jg.nivel, jg.jogador.pontuacao);
      DesenharHitscan(&jg.jogador);
      EndDrawing();

      ++contadorDeFrames;
    }
  }

  SalvarMapa(jg.mapa, "mapa.txt");

  CloseWindow();
}
