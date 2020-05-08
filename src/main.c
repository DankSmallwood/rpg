#include <stdio.h>
#include "character.h"

int main() {
  Character bob = {
    .base_stats = {
      .vigor = 10,
      .finesse = 12,
      .intellect = 4,
      .spirit = 10
    }
  };

  printf("%d\n", bob.base_stats.vigor);

  return 0;
}
