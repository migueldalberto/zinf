#include "mapa.h"
#include "defs.h"

#include <stdio.h>

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

void DesenharMapa(Mapa m) {
  for (int i = 0; i < 16; ++i) {
    for (int j = 0; j < 24; ++j) {
      Rectangle dst = { j * 32 * ESCALA, i * 32 * ESCALA + ALTURA_STATUS, 32 * ESCALA, 32 * ESCALA};
      DrawTexturePro(m.textura, m.sources[i][j],
		     dst, (Vector2){ 0,0 }, 0.0, WHITE);
    }
  }
}
