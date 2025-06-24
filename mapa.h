#ifndef MAPA_H
#define MAPA_H

#include <raylib.h>
#include <stdlib.h>

typedef struct {
  enum {
    GRAMA,
    PEDRA
  } tiles[16][24];
  Rectangle sources[16][24];
  Vector2 posicaoJogador;
  Vector2 posicaoMonstros[10];
  Vector2 posicaoEspada;
  Vector2 posicaoVidas[5];
  int nDeMonstros;
  int nDeVidas;
} Mapa;

Vector2 ConverterPosicaoMapaTela(Vector2);
int CarregarMapa(Mapa*, char*);
int SalvarMapa(Mapa, char*);
void MapaGerarRetangulos(Mapa*);
void DesenharMapa(Mapa, Texture2D*);

#endif
