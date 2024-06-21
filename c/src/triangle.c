#include "triangle.h"
#include "display.h"
#include "lighting.h"
#include "texture.h"
#include "vector.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

void vec2_swap(vec2_t *a, vec2_t *b) {
  vec2_t tmp = *a;
  *a = *b;
  *b = tmp;
}

void vec4_swap(vec4_t *a, vec4_t *b) {
  vec4_t tmp = *a;
  *a = *b;
  *b = tmp;
}

void float_swap(float *a, float *b) {
  float tmp = *a;
  *a = *b;
  *b = tmp;
}
void int_swap(int *a, int *b) {
  int tmp = *a;
  *a = *b;
  *b = tmp;
}

void tex2_swap(tex2_t *a, tex2_t *b) {
  tex2_t tmp = *a;
  *a = *b;
  *b = tmp;
}

float lerp(float v0, float v1, float t) { return (1 - t) * v0 + t * v1; }

float two_points_distance(vec4_t a, vec4_t b) {
  return sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

void draw_texel(int x, int y, uint32_t *texture, triangle_t triangle) {
  vec3_t weights = barycentric_weights(
      (vec2_t){triangle.points[0].x, triangle.points[0].y},
      (vec2_t){triangle.points[1].x, triangle.points[1].y},
      (vec2_t){triangle.points[2].x, triangle.points[2].y}, (vec2_t){x, y});

  float interpolated_u;
  float interpolated_v;
  float interpolated_reciprocal_w;

  interpolated_u =
      (triangle.texcoords[0].u / triangle.points[0].w) * weights.x +
      (triangle.texcoords[1].u / triangle.points[1].w) * weights.y +
      (triangle.texcoords[2].u / triangle.points[2].w) * weights.z;
  interpolated_v =
      (triangle.texcoords[0].v / triangle.points[0].w) * weights.x +
      (triangle.texcoords[1].v / triangle.points[1].w) * weights.y +
      (triangle.texcoords[2].v / triangle.points[2].w) * weights.z;

  interpolated_reciprocal_w = (1 / triangle.points[0].w) * weights.x +
                              (1 / triangle.points[1].w) * weights.y +
                              (1 / triangle.points[2].w) * weights.z;

  interpolated_u /= interpolated_reciprocal_w;
  interpolated_v /= interpolated_reciprocal_w;

  int tex_x = abs((int)(interpolated_u * (texture_width)));
  int tex_y = abs((int)(interpolated_v * (texture_height)));
  float intensity = triangle.intensities[0] * weights.x +
                    triangle.intensities[1] * weights.y +
                    triangle.intensities[2] * weights.z;
  interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;

  if (window_width * y + x >= 0 &&
      window_width * y + x < window_width * window_height &&
      z_buffer[window_width * y + x] >= interpolated_reciprocal_w) {
    z_buffer[window_width * y + x] = interpolated_reciprocal_w;
    uint32_t color = texture == NULL
                         ? triangle.color
                         : texture[((texture_width * tex_y) + tex_x) %
                                   (texture_height * texture_width)];
    draw_pixel(x, y, light_apply_intensity(color, intensity));
  }
}

void draw_textured_triangle(triangle_t triangle, uint32_t *texture) {
  if (triangle.points[0].y > triangle.points[1].y) {
    vec4_swap(&triangle.points[0], &triangle.points[1]);
    float_swap(&triangle.intensities[0], &triangle.intensities[1]);
    tex2_swap(&triangle.texcoords[0], &triangle.texcoords[1]);
  }
  if (triangle.points[1].y > triangle.points[2].y) {
    vec4_swap(&triangle.points[1], &triangle.points[2]);
    float_swap(&triangle.intensities[2], &triangle.intensities[1]);
    tex2_swap(&triangle.texcoords[1], &triangle.texcoords[2]);
  }
  if (triangle.points[0].y > triangle.points[1].y) {
    vec4_swap(&triangle.points[0], &triangle.points[1]);
    float_swap(&triangle.intensities[0], &triangle.intensities[1]);
    tex2_swap(&triangle.texcoords[0], &triangle.texcoords[1]);
  }

  triangle.texcoords[0].v = 1 - triangle.texcoords[0].v;
  triangle.texcoords[1].v = 1 - triangle.texcoords[1].v;
  triangle.texcoords[2].v = 1 - triangle.texcoords[2].v;
  // flat bottom triangle
  float inv_slope1 = 0;
  float inv_slope2 = 0;

  if (triangle.points[0].y != triangle.points[1].y)
    inv_slope1 = (float)(triangle.points[1].x - triangle.points[0].x) /
                 fabs(triangle.points[1].y - triangle.points[0].y);
  if (triangle.points[0].y != triangle.points[2].y)
    inv_slope2 = (float)(triangle.points[2].x - triangle.points[0].x) /
                 fabs(triangle.points[2].y - triangle.points[0].y);

  if (triangle.points[1].y != triangle.points[0].y)
    for (float y = triangle.points[0].y; y <= triangle.points[1].y; y++) {
      int x_start =
          triangle.points[1].x + (y - triangle.points[1].y) * inv_slope1;
      int x_end =
          triangle.points[0].x + (y - triangle.points[0].y) * inv_slope2;
      if (x_start > x_end) {
        int_swap(&x_start, &x_end);
      }
      for (float x = x_start; x <= x_end; x++) {
        draw_texel(x, y, texture, triangle);
      }
    }

  // flat top triangle
  inv_slope1 = 0;
  inv_slope2 = 0;

  if (triangle.points[2].y != triangle.points[1].y)
    inv_slope1 = (float)(triangle.points[2].x - triangle.points[1].x) /
                 fabs(triangle.points[2].y - triangle.points[1].y);
  if (triangle.points[0].y != triangle.points[2].y)
    inv_slope2 = (float)(triangle.points[2].x - triangle.points[0].x) /
                 fabs(triangle.points[2].y - triangle.points[0].y);

  if (triangle.points[2].y != triangle.points[1].y)
    for (float y = triangle.points[1].y; y <= triangle.points[2].y; y++) {
      int x_start =
          triangle.points[1].x + (y - triangle.points[1].y) * inv_slope1;
      int x_end =
          triangle.points[0].x + (y - triangle.points[0].y) * inv_slope2;
      if (x_start > x_end) {
        int_swap(&x_start, &x_end);
      }
      for (float x = x_start; x <= x_end; x++) {
        draw_texel(x, y, texture, triangle);
      }
    }
};
