#ifndef MAPA_H
#define MAPA_H

#include <raylib.h>
#include <stdlib.h>

#define ENTIDADES_POS_CAP 100

typedef enum {
  JOGADOR,
  SLIME_VERDE,
  VIDA_EXTRA,
  ESPADA,
  PEDRA,
  SLIME_VERDE_MORTE
} EntidadeTipo;

typedef struct {
  enum {
    GRAMA
  } tiles[16][24];
  Rectangle sources[16][24];
  Vector2 posicaoJogador;
  struct {
    EntidadeTipo t;
    Vector2 p;
  } entidadesPos[ENTIDADES_POS_CAP];
  int entidadesTam;
} Mapa;

Vector2 ConverterPosicaoMapaTela(Vector2);
int CarregarMapa(Mapa*, char*);
int SalvarMapa(Mapa, char*);
void MapaGerarRetangulos(Mapa*);
void DesenharMapa(Mapa, Texture2D*);
void GerarMapaAleatorio(Mapa*, int);

#endif
