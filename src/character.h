#ifndef CHARACTER_H
#define CHARACTER_H

#include "config.h"

typedef struct {
  char *name;
#include "stats/__stats.z"
} Character;

#endif
