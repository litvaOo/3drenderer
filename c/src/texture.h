#ifndef TEXTURE_H
#define TEXTURE_H
#include "upng.h"
#include "vector.h"
#include <stdint.h>
typedef struct {
  float u;
  float v;
} tex2_t;

extern int texture_width;
extern int texture_height;

extern upng_t *png_texture;
extern const uint8_t REDBRICK_TEXTURE[];

extern uint32_t *mesh_texture;
vec3_t barycentric_weights(vec2_t a, vec2_t b, vec2_t c, vec2_t p);
void load_png_texture_data(char *filename);
tex2_t tex2_clone(tex2_t *t);
#endif // !TEXTURE_H
