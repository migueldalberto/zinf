#include "sprite.h"

#include "defs.h"

void DesenharSprite(Sprite s) {
  DrawTexturePro(*s.textura, s.src, s.dst, (Vector2){ 0.0, 0.0 }, 0.0, WHITE);
}
