#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "lighting.h"
#include "texture.h"
#include "upng.h"
#include "vector.h"
#include <stdint.h>
typedef struct {
  int a;
  int b;
  int c;
  uint32_t color;
  int normals[3];
  int texcoords[3];
} face_t;

typedef struct {
  vec4_t points[3];
  tex2_t texcoords[3];
  uint32_t color;
  float intensities[3];
  upng_t *texture;
} triangle_t;

void draw_textured_triangle(triangle_t triangle);

#endif
