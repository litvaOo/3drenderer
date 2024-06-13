#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "lighting.h"
#include "texture.h"
#include "vector.h"
#include <stdint.h>
typedef struct {
  int a;
  int b;
  int c;
  uint32_t color;
  int normals[3];
  tex2_t a_uv;
  tex2_t b_uv;
  tex2_t c_uv;
} face_t;

typedef struct {
  vec4_t points[3];
  tex2_t texcoords[3];
  uint32_t color;
  float avg_depth;
  float intensities[3];
} triangle_t;

void draw_filled_triangle(triangle_t triangle);
void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2,
                               uint32_t color, float *intensities);
void fill_flat_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2,
                            uint32_t color, float *intensities);
void draw_textured_triangle(triangle_t triangle, uint32_t *texture);

#endif
