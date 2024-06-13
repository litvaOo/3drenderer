#ifndef TEXTURE_H
#define TEXTURE_H
#include "vector.h"
#include <stdint.h>
typedef struct {
  float u;
  float v;
} tex2_t;

extern int texture_width;
extern int texture_height;

extern const uint8_t REDBRICK_TEXTURE[];

extern uint32_t *mesh_texture;
vec3_t barycentric_weights(vec2_t a, vec2_t b, vec2_t c, vec2_t p);
#endif // !TEXTURE_H
