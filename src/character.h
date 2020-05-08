#ifndef CHARACTER_H
#define CHARACTER_H

#include "config.h"

typedef struct {
#include "stats/__stats.z"
} Stats;

typedef struct {
  Stats base_stats;
} Character;

Stats character_get_derived_stats(Character *c);

#endif
