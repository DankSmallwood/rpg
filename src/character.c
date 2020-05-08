#include "character.h"

Stats character_get_derived_stats(Character *c) {
  return c->base_stats;
}
