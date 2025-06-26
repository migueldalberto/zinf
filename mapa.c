#include "mapa.h"
#include "defs.h"

#include <stdio.h>
#include <raymath.h>

Vector2 ConverterPosicaoMapaTela(Vector2 m) {
  return Vector2Add(Vector2Scale(m, 32 * ESCALA), (Vector2){ 0, ALTURA_STATUS});
}

void AdicionarEntidadePos(Mapa *m, EntidadeTipo t, int x, int y) {
  m->entidadesPos[m->entidadesTam].t = t;
  m->entidadesPos[m->entidadesTam].p.x = x;
  m->entidadesPos[m->entidadesTam].p.y = y;
  m->entidadesTam++;
}

int CarregarMapa(Mapa* m, char* nomeArq) {
  FILE* f = fopen(nomeArq, "r");

  if (f == NULL)
    return 1;

  char c;
  int x = 0;
  int y = 0;
  m->entidadesTam = 0;

  while ((c=fgetc(f)) != EOF && y < 16) {
    int tile = GRAMA;
    switch (c) {
    case '\n':
      x = 0;
      ++y;
    case '\r':
      continue;
      break;
    case 'J':
      m->posicaoJogador = (Vector2){ x, y };
      break;
    case 'V':
      puts("VIDA NO MAPA");
      AdicionarEntidadePos(m, VIDA_EXTRA, x, y);
      break;
    case 'M':
      puts("SLIME NO MAPA");
      AdicionarEntidadePos(m, SLIME_VERDE, x, y);
      break;
    case 'E':
      AdicionarEntidadePos(m, ESPADA, x, y);
      break;
    case 'P':
      AdicionarEntidadePos(m, PEDRA, x, y);
      break;
    default: break;
    }

    m->tiles[y][x] = tile;
    ++x;
  }

  fclose(f);

  MapaGerarRetangulos(m);
  return 0;
}

int SalvarMapa(Mapa m, char* nArq) {
  FILE* f = fopen(nArq, "w");
  if (f == NULL)
    return 1;

  for (int i = 0; i < 16; ++i) {
    for (int j = 0; j < 24; ++j) {
      char c;
      switch (m.tiles[i][j]) {
      case GRAMA: c = ' '; break;
      }

      if (j == m.posicaoJogador.x && i == m.posicaoJogador.y)
	c = 'J';

      for (int k = 0; k < ENTIDADES_POS_CAP; ++k) {
	if (m.entidadesPos[k].p.x == j && m.entidadesPos[k].p.y == i) {
	  switch(m.entidadesPos[k].t) {
	  case SLIME_VERDE: c = 'M'; break;
	  case ESPADA: c = 'E'; break;
	  case VIDA_EXTRA: c = 'V'; break;
	  default: c = ' '; break;
	  }
	}
      }

      if (fputc(c, f) == EOF) {
	fclose(f);
	return 1;
      }
	
    }
    if (fputc('\n', f) == EOF) {
      fclose(f);
      return 1;
    }
  }

  fclose(f);
  return 0;
}

void MapaGerarRetangulos(Mapa* m) {
  for (int i = 0; i < 16; ++i) {
    for (int j = 0; j < 24; ++j) {
      int r = rand();
      switch(m->tiles[i][j]) {
      case GRAMA:
      default:
	m->sources[i][j] = (Rectangle){
	  .x = (r % 6) * 32,
	  .y = 0,
	  .width = 32,
	  .height = 32
	};
	break;
      }
    }
  }
}

void DesenharMapa(Mapa m, Texture2D* tileset) {
  for (int i = 0; i < 16; ++i) {
    for (int j = 0; j < 24; ++j) {
      Rectangle dst = { j * 32 * ESCALA, i * 32 * ESCALA + ALTURA_STATUS, 32 * ESCALA, 32 * ESCALA};
      DrawTexturePro(*tileset, m.sources[i][j],
		     dst, (Vector2){ 0,0 }, 0.0, WHITE);
    }
  }
}

void GerarMapaAleatorio(Mapa* m, int n) {
  m->entidadesTam = 0;
  
  for (int i = 0; i < 16; ++i)
    for (int j = 0; j < 24; ++j)
      m->tiles[i][j] = GRAMA;

  int espadaX = rand() % 24;
  int espadaY = rand() % 16;
  AdicionarEntidadePos(m, ESPADA, espadaX, espadaY);
  m->posicaoJogador.x = rand() % 24;
  m->posicaoJogador.y = rand() % 16;

  int nDePedras = 16 * 24 / 10 / 3;
  for (int i = 0; i < nDePedras; ++i) {
    int x, y;
    do {
      x = rand() % 24;
      y = rand() % 16;
    } while((x == m->posicaoJogador.x && y == m->posicaoJogador.y) ||
	    (x == espadaX && y == espadaY));
    AdicionarEntidadePos(m, PEDRA, x, y);
  }

  MapaGerarRetangulos(m);

  int nDeMonstros = (rand() % 10) + 1;
  for (int i = 0; i < nDeMonstros; ++i) {
    int x, y;
    do {
      x = rand() % 24;
      y = rand() % 16;
    } while((x == m->posicaoJogador.x && y == m->posicaoJogador.y) ||
	    (x == espadaX && y == espadaY));
    AdicionarEntidadePos(m, SLIME_VERDE, x, y);
  }

  int nDeVidas = rand() % 6;
  
  for (int i = 0; i < nDeVidas; ++i) 
    AdicionarEntidadePos(m, VIDA_EXTRA, rand() % 24, rand() % 16);
}
