#ifndef RANKING_H
#define RANKING_H

typedef struct {
  char nome[20];
  int score;
} RankingLinha;

typedef struct {
  RankingLinha* linhas;
  int tam;
  int cap;
} Ranking;

int InicializarRanking(Ranking*);
int RankingAdd(Ranking*, RankingLinha);
int CarregarRanking(Ranking*, char*);
int SalvarRanking(Ranking, char*);
void FreeRanking(Ranking);

#endif
