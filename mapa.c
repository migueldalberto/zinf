#include "mapa.h"
#include "defs.h"

#include <stdio.h>
#include <raymath.h>

Vector2 ConverterPosicaoMapaTela(Vector2 m) {
  return Vector2Add(Vector2Scale(m, 32), (Vector2){ 0, ALTURA_STATUS});
}

int CarregarMapa(Mapa* m, char* nomeArq) {
  FILE* f = fopen(nomeArq, "r");

  if (f == NULL)
    return 1;

  char c;
  int x = 0;
  int y = 0;
  m->nDeVidas = 0;
  m->nDeMonstros = 0;
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
      m->posicaoVidas[m->nDeVidas] = (Vector2){ x, y };
      m->nDeVidas += 1;
      break;
    case 'M':
      m->posicaoMonstros[m->nDeMonstros] = (Vector2){ x, y };
      m->nDeMonstros += 1;
      break;
    case 'E':
      m->posicaoEspada = (Vector2){ x, y };
      break;
    case 'P':
      tile = PEDRA;
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
      case PEDRA: c = 'P'; break;
      }

      if (j == m.posicaoJogador.x && i == m.posicaoJogador.y)
	c = 'J';

      for (int k = 0; k < 10; ++k)
	if (m.posicaoMonstros[k].x == j && m.posicaoMonstros[k].y == i)
	  c = 'M';

      if (m.posicaoEspada.x == j && m.posicaoEspada.y == i)
	c = 'E';

      for (int k = 0; k < 5; ++k)
	if (m.posicaoVidas[5].x == j && m.posicaoVidas[5].y == i)
	  c = 'V';

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
      case PEDRA:
	m->sources[i][j] = (Rectangle) {
	  .x = (r % 2) * 32,
	  .y = 32,
	  .width = 32,
	  .height = 32
	};
	break;
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

  m->posicaoJogador = (Vector2){ 8, 6 };

  m->nDeMonstros = (rand() % 10) + 1;
  for (int i = 0; i < m->nDeMonstros; ++i) {
    m->posicaoMonstros[i].x = (rand() % 24);
    m->posicaoMonstros[i].y = (rand() % 16);
  }

  m->nDeVidas = (rand() % 5) + 1;

  for (int i = 0; i < m->nDeVidas; ++i) {
    m->posicaoVidas[i].x = (rand() % 24);
    m->posicaoVidas[i].y = (rand() % 16);
  }

  m->posicaoEspada.x = rand() % 24;
  m->posicaoEspada.y = rand() % 16;
}
