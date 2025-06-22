#include "menu.h"

#include <string.h>

Botao NovoBotao(char* txt, float x, float y, Color cb, Color ct) {
  return (Botao){
    .txt = txt,
    .ret = (Rectangle){
      .x = x,
      .y = y,
      .width = 60 * (strlen(txt) - 1),
      .height = ALTURA / 10
    },
    .cb = cb,
    .ct = ct
  };
}

void DesenharBotao(Botao b) {
  DrawRectangleRec(b.ret, b.cb);
  DrawText(b.txt, b.ret.x + b.ret.width / 10, b.ret.y + b.ret.height / 10,
	   b.ret.height * 8 / 10, b.ct
	   );
}
