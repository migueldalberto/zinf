#ifndef MENU_H
#define MENU_H

#include <raylib.h>
#include "defs.h"

typedef struct {
  char* txt;
  Rectangle ret;
  Color cb;
  Color ct;
} Botao;

Botao NovoBotao(char*, float, float, Color cb, Color ct);
void DesenharBotao(Botao);
bool CliqueNoBotao(Botao);

#endif
