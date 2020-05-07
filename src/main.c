#include <stdio.h>
#include "character.h"

int main() {
  Character bob = {
    .name = "Bob",
    .vigor = 10,
    .finesse = 12,
    .intellect = 4,
    .spirit = 10
  };

  printf("%d\n", bob.vigor);

  return 0;
}
