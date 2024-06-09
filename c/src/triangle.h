#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "lighting.h"
#include "vector.h"
#include <stdint.h>
typedef struct {
  int a;
  int b;
  int c;
  uint32_t color;
  int normals[3];
} face_t;

typedef struct {
  vec2_t points[3];
  uint32_t color;
  float avg_depth;
  float intensities[3];
} triangle_t;

void draw_filled_triangle(triangle_t triangle);
void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2,
                               uint32_t color, float *intensities);
void fill_flat_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2,
                            uint32_t color, float *intensities);
#endif
