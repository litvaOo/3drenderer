#ifndef TEXTURE_H
#define TEXTURE_H
#include "upng.h"
#include "vector.h"
#include <stdint.h>
typedef struct {
  float u;
  float v;
} tex2_t;

vec3_t barycentric_weights(vec2_t a, vec2_t b, vec2_t c, vec2_t p);
tex2_t tex2_clone(tex2_t *t);
#endif // !TEXTURE_H
