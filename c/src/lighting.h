#ifndef LIGHTING_H
#define LIGHTING_H

#include "vector.h"
#include <stdint.h>
typedef struct {
  vec3_t direction;
} light_t;

uint32_t light_apply_intensity(uint32_t original_color,
                               float percentage_factor);
#endif
