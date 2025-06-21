#include "mapa.h"
#include "defs.h"

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
