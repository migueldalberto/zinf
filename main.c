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
#include <assert.h>

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
  Texture2D vidaExtra;
  Texture2D espada;
  Texture2D pedra;
  Texture2D semTextura;
} Texturas;

#define ENTIDADES_CAP 100

typedef struct {
  Ranking ranking;
  Entidade jogador;
  Entidade entidades[ENTIDADES_CAP];
  int entidadesTam;
  int nivel;
  Cena cena;
  Mapa mapa;
  bool gerarNiveisAleatorios;
  Texturas* texturas;
} Jogo;

void RemoverEntidade(Jogo* jg, int i) {
  for (int j = i; j < jg->entidadesTam - 1; ++j) {
    jg->entidades[j] = jg->entidades[j + 1];
  }
  jg->entidadesTam -= 1;
}

int AdicionarEntidade(Jogo* jg, EntidadeTipo tipo, Vector2 posicao) {
    assert(jg->entidadesTam < ENTIDADES_CAP);

  Texture2D* textura;

  switch (tipo) {
  case SLIME_VERDE: textura = &jg->texturas->slime; break;
  case VIDA_EXTRA: textura = &jg->texturas->vidaExtra; break;
  case ESPADA: textura = &jg->texturas->espada; break;
  case PEDRA: textura = &jg->texturas->pedra; break;
  case SLIME_VERDE_MORTE: textura = &jg->texturas->slimeMorre; break;
  default: textura = &jg->texturas->semTextura; break;
  }

  InicializarEntidade(&(jg->entidades[jg->entidadesTam]), tipo, textura,
		      posicao);
  jg->entidadesTam += 1;
  return jg->entidadesTam - 1;
}

int AdicionarEntidadePosMapa(Jogo* jg, EntidadeTipo tipo, Vector2 posicao) {
  return AdicionarEntidade(jg, tipo, ConverterPosicaoMapaTela(posicao));
}

void ProximoNivel(Jogo* jg) {
  ++jg->nivel;
  if (jg->gerarNiveisAleatorios) {
    GerarMapaAleatorio(&jg->mapa, jg->nivel);
  } else if (niveis[jg->nivel] == NULL) {
    jg->gerarNiveisAleatorios = true;
    GerarMapaAleatorio(&jg->mapa, jg->nivel);
  } else
    CarregarMapa(&jg->mapa, niveis[jg->nivel]);
  
  jg->jogador.espada = false;
  
  jg->entidadesTam = 0;
  
  for (int i = 0; i < jg->mapa.entidadesTam; ++i)
    AdicionarEntidadePosMapa(jg, jg->mapa.entidadesPos[i].t, jg->mapa.entidadesPos[i].p);
  

}

void InicioDeJogo(Jogo* jg) {
  if (CarregarMapa(&jg->mapa, niveis[jg->nivel])) {
    jg->gerarNiveisAleatorios = true;
    GerarMapaAleatorio(&jg->mapa, jg->nivel);
  }

  InicializarEntidade(&jg->jogador,
		      JOGADOR,
		      jg->texturas->jogador,
		      ConverterPosicaoMapaTela(jg->mapa.posicaoJogador));

  jg->entidadesTam = 0;
  for (int i = 0; i < jg->mapa.entidadesTam; ++i)
    AdicionarEntidadePosMapa(jg, jg->mapa.entidadesPos[i].t, jg->mapa.entidadesPos[i].p);
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
      int y = 48 + (i * 36);
      DrawText(j->ranking.linhas[i].nome, LARGURA / 2 - (20 * cw), y, 32, c);
      DrawText(TextFormat("%9d", j->ranking.linhas[i].score), LARGURA / 2 + (9 * cw), y, 32, c);
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

  for(int i = 0; i < j->entidadesTam; ++i)
    DesenharEntidade(j->entidades[i]);
  
  DesenharStatus(j->jogador.vida, j->nivel, j->jogador.pontuacao);
  DrawRectangle(0, 0, LARGURA, ALTURA, Fade(BLACK, 0.8));
  DrawText("Você morreu", LARGURA / 2 - (48 * 5.5), 96, 96, RED);
  DrawText(TextFormat("Sua pontuação: %06d", j->jogador.pontuacao), LARGURA / 2 - (32 * 10.5), 192, 64, YELLOW);
  DesenharBotao(bMenuPrincipal);
  DesenharBotao(bSalvarPontuacao);
  EndDrawing();
}

// função pra ordenar as entidades de acordo com o valor Y de sua posição
int comp(const void* a, const void* b) {
  return ((Entidade*) a)->posicao.y > ((Entidade*) b)->posicao.y;
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
    .slimeMorre = LoadTexture("assets/slime_verde_morre.png"),
    .vidaExtra = LoadTexture("assets/vida_extra.png"),
    .espada = LoadTexture("assets/espada.png"),
    .pedra = LoadTexture("assets/pedra.png"),
  };

  Jogo jg;
  jg.texturas = &texturas;
  
  jg.cena = MENU_PRINCIPAL;

  SetTargetFPS(30);

  int contadorDeFrames = 0;
  int velocidadeAnim = 5;

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
      LerControles(&jg.jogador, jg.entidades, jg.entidadesTam);
      jg.jogador.posicao = Vector2Add(jg.jogador.posicao, jg.jogador.deslocamento);
      AtualizarHitbox(&jg.jogador);
      AtualizarEfeitos(&jg.jogador);
      if (ForaDoMapa(jg.jogador))
	ReverterMovimento(&jg.jogador);

      int nDeMonstros = 0;

      for (int i = 0; i < jg.entidadesTam; ++i) {
	Entidade* e = &jg.entidades[i];
	AtualizarHitbox(e);

	if (e->tipo == SLIME_VERDE) {
	  if (e->vida == 0) {
	    jg.jogador.pontuacao += e->pontuacao;
	    int ind = AdicionarEntidade(&jg, SLIME_VERDE_MORTE, e->posicao);
	    AtualizarSprite(&jg.entidades[ind]);
	    RemoverEntidade(&jg, i);
	  }
	  ++nDeMonstros;
	  SlimeOrientacao(e, jg.jogador.hitbox);
	  SlimeMove(e);
	  if (ForaDoMapa(*e))
	    ReverterMovimento(e);
	  
	  AtualizarEfeitos(e);
	  
	  if (ChecarColisao(*e, jg.jogador) && !(jg.jogador.efeitos & DANO)) {
	    jg.jogador.efeitos |= DANO;
	    // "knockback"
	    /* jg.jogador.posicao = Vector2Add(jg.jogador.posicao, Vector2Scale(e->deslocamento, 2)); */
	    jg.jogador.vida -= 1;
	    AtualizarHitbox(&jg.jogador);
	  }
	} else if (e->tipo == ESPADA) {
	  if (ChecarColisao(*e, jg.jogador)) {
	    jg.jogador.espada = true;
	    RemoverEntidade(&jg, i);
	  }
	} else if (e->tipo == VIDA_EXTRA) {
	  if (ChecarColisao(*e, jg.jogador)) {
	    jg.jogador.vida += 1;
	    RemoverEntidade(&jg, i);
	  }
	}

	if (e->obstaculo) {
	  if (ChecarColisao(*e, jg.jogador))
	    ReverterMovimento(&jg.jogador);
	  for (int j = 0; j < jg.entidadesTam; ++j) {
	    if (j == i) continue;
	    if (ChecarColisao(*e, jg.entidades[j])) {
	      ReverterMovimento(&jg.entidades[j]);
	    }
	  }
	}
      }

      if (nDeMonstros == 0)
	ProximoNivel(&jg);
      
      if (jg.jogador.vida == 0)
	TrocarCena(&jg, FIM_DE_JOGO);

      if (contadorDeFrames >= 30 / velocidadeAnim) {
	contadorDeFrames = 0;

	Sprite *s = &jg.jogador.sprites[jg.jogador.spriteAtual];
	ProximoFrame(s);
      
	for (int i = 0; i < jg.entidadesTam; ++i) {
	  Sprite *sm = jg.entidades[i].sprites;
	  ProximoFrame(sm);

	  if (sm->frameAtual == 4 && jg.entidades[i].tipo == SLIME_VERDE_MORTE)
	    RemoverEntidade(&jg, i);
	}
      }
    
      for (int i = 0; i < jg.entidadesTam; ++i) {
	Entidade* e = &jg.entidades[i];
	AtualizarSprite(e);
      }
      AtualizarSprite(&jg.jogador);

    
      BeginDrawing();
      ClearBackground(WHITE);
      DesenharMapa(jg.mapa, &texturas.tileset);
      /* DesenharEntidade(jg.jogador); */

      qsort(jg.entidades, jg.entidadesTam, sizeof(Entidade), comp);

      bool jogadorDesenhado = false;
      for (int i = 0; i < jg.entidadesTam; ++i) {
	if (i == jg.entidadesTam - 1 && !jogadorDesenhado) {
	  DesenharEntidade(jg.jogador);
	} else if (jg.jogador.posicao.y < jg.entidades[i].posicao.y &&
		   jg.jogador.posicao.y >= jg.entidades[i + 1].posicao.y) {
	  jogadorDesenhado = true;
	  DesenharEntidade(jg.jogador);
	}
	
	DesenharEntidade(jg.entidades[i]);
      }
    
      DesenharStatus(jg.jogador.vida, jg.nivel, jg.jogador.pontuacao);
      /* DesenharHitscan(&jg.jogador); */
      EndDrawing();

      ++contadorDeFrames;
    }
  }

  /* SalvarMapa(jg.mapa, "mapa.txt"); */

  CloseWindow();
}
