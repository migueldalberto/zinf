#include "ranking.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

int InicializarRanking(Ranking* r) {
  assert(r != NULL);
  
  r->tam = 0;
  r->cap = 2;
  r->linhas = (RankingLinha*) malloc(sizeof(RankingLinha) * r->cap);
  if (r->linhas == NULL) 
    return 1;

  return 0;
}

int RankingAdd(Ranking* r, RankingLinha l) {
  assert(r != NULL);
  assert(r->tam <= r->cap);
  
  if (r->tam == r->cap) {
    RankingLinha* new = (RankingLinha*) malloc(sizeof(RankingLinha) * r->cap * 2);
    if (new == NULL)
      return 1;

    for (int i = 0; i < r->tam; ++i)
      new[i] = r->linhas[i];

    free(r->linhas);
    r->linhas = new;
    r->cap *= 2;
  }

  r->linhas[r->tam++] = l;
  return 0;
}

int CarregarRanking(Ranking* r, char* nomeArq) {
  if (InicializarRanking(r) == 1) 
    return 1;

  FILE* f = fopen(nomeArq, "rb");

  if (f == NULL)
    return 1;

  while (!feof(f)) {
    RankingLinha l;
    
    fread(&l, sizeof(RankingLinha), 1, f);
    
    if (RankingAdd(r, l) == 1) {
      fclose(f);
      return 1;
    }
  }
  
  fclose(f);
  return 0;
}

int SalvarRanking(Ranking r, char* nomeArq) {
  FILE* f = fopen(nomeArq, "wb");

  if (f == NULL)
    return 1;

  if (fwrite(r.linhas, sizeof(RankingLinha), r.tam, f) != (unsigned int) r.tam) {
    fclose(f);
    return 1;
  }

  fclose(f);
  return 0;
}

void FreeRanking(Ranking r) {
  r.cap = 0;
  r.tam = 0;
  free(r.linhas);
}
