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

uint32_t get_texel(float x, float y, uint32_t *texture, triangle_t triangle) {
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
  return texture[((texture_width * tex_y) + tex_x) %
                 (texture_height * texture_width)];
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
      int delta = x_end - x_start;
      float t1 = two_points_distance(triangle.points[0], (vec4_t){x_start, y}) /
                 two_points_distance(triangle.points[0], triangle.points[1]);

      float i1 = lerp(triangle.intensities[0], triangle.intensities[1], t1);
      float t2 = two_points_distance(triangle.points[0], (vec4_t){x_end, y}) /
                 two_points_distance(triangle.points[0], triangle.points[2]);

      float i2 = lerp(triangle.intensities[0], triangle.intensities[2], t2);
      for (float x = x_start; x < x_end; x++) {
        float t = (float)(x - x_start) / delta;
        float i3 = lerp(i1, i2, t);
        uint32_t new_color =
            light_apply_intensity(get_texel(x, y, texture, triangle), i3);
        draw_pixel(x, y, new_color);
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
      int delta = x_end - x_start;
      float t1 = two_points_distance(triangle.points[0], (vec4_t){x_start, y}) /
                 two_points_distance(triangle.points[0], triangle.points[1]);

      float i1 = lerp(triangle.intensities[0], triangle.intensities[1], t1);
      float t2 = two_points_distance(triangle.points[0], (vec4_t){x_end, y}) /
                 two_points_distance(triangle.points[0], triangle.points[2]);

      float i2 = lerp(triangle.intensities[0], triangle.intensities[2], t2);
      for (float x = x_start; x < x_end; x++) {
        float t = (float)(x - x_start) / delta;
        float i3 = lerp(i1, i2, t);
        uint32_t new_color =
            light_apply_intensity(get_texel(x, y, texture, triangle), i3);
        draw_pixel(x, y, new_color);
      }
    }
};

void draw_filled_triangle(triangle_t triangle) {
  if (triangle.points[0].y > triangle.points[1].y) {
    vec4_swap(&triangle.points[0], &triangle.points[1]);
    float_swap(&triangle.intensities[0], &triangle.intensities[1]);
  }
  if (triangle.points[1].y > triangle.points[2].y) {
    vec4_swap(&triangle.points[1], &triangle.points[2]);
    float_swap(&triangle.intensities[2], &triangle.intensities[1]);
  }
  if (triangle.points[0].y > triangle.points[1].y) {
    vec4_swap(&triangle.points[0], &triangle.points[1]);
    float_swap(&triangle.intensities[0], &triangle.intensities[1]);
  }

  if (triangle.points[1].y == triangle.points[2].y) {
    fill_flat_bottom_triangle(
        triangle.points[0].x, triangle.points[0].y, triangle.points[1].x,
        triangle.points[1].y, triangle.points[2].x, triangle.points[2].y,
        triangle.color,
        (float[]){triangle.intensities[0], triangle.intensities[1],
                  triangle.intensities[2]});
  } else if (triangle.points[1].y == triangle.points[0].y) {
    fill_flat_top_triangle(
        triangle.points[0].x, triangle.points[0].y, triangle.points[1].x,
        triangle.points[1].y, triangle.points[2].x, triangle.points[2].y,
        triangle.color,
        (float[]){triangle.intensities[0], triangle.intensities[1],
                  triangle.intensities[2]});
  } else {
    int My = triangle.points[1].y;
    int Mx = (((triangle.points[2].x - triangle.points[0].x) *
               (triangle.points[1].y - triangle.points[0].y)) /
              (triangle.points[2].y - triangle.points[0].y)) +
             triangle.points[0].x;

    float t = two_points_distance(triangle.points[0], (vec4_t){Mx, My}) /
              two_points_distance(triangle.points[0], triangle.points[2]);

    float Mi = lerp(triangle.intensities[0], triangle.intensities[2], t);

    fill_flat_bottom_triangle(
        triangle.points[0].x, triangle.points[0].y, triangle.points[1].x,
        triangle.points[1].y, Mx, My, triangle.color,
        (float[]){triangle.intensities[0], triangle.intensities[1], Mi});

    fill_flat_top_triangle(
        triangle.points[1].x, triangle.points[1].y, Mx, My,
        triangle.points[2].x, triangle.points[2].y, triangle.color,
        (float[]){triangle.intensities[1], Mi, triangle.intensities[2]});
  }
}

void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2,
                               uint32_t color, float intensities[]) {
  float slope1 = (float)(x1 - x0) / (y1 - y0);
  float slope2 = (float)(x2 - x0) / (y2 - y0);

  float x_start = x0;
  float x_end = x0;

  for (int y = y0; y <= y2; y++) {
    int delta = x_end - x_start;
    int change = delta / abs(delta);
    float t1 = two_points_distance((vec4_t){x0, y0}, (vec4_t){x_start, y}) /
               two_points_distance((vec4_t){x0, y0}, (vec4_t){x1, y1});

    float i1 = lerp(intensities[0], intensities[1], t1);
    float t2 = two_points_distance((vec4_t){x0, y0}, (vec4_t){x_end, y}) /
               two_points_distance((vec4_t){x0, y0}, (vec4_t){x2, y2});

    float i2 = lerp(intensities[0], intensities[2], t2);
    for (int i = 0; i <= abs(delta) + 1; i++) {
      float t = (float)i / delta;
      float i3 = lerp(i1, i2, t);
      uint32_t new_color = light_apply_intensity(color, i3);
      draw_pixel(x_start + i * change, y, new_color);
    }
    x_start += slope1;
    x_end += slope2;
  }
}

void fill_flat_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2,
                            uint32_t color, float intensities[]) {
  float slope1 = (float)(x2 - x0) / (y2 - y0);
  float slope2 = (float)(x2 - x1) / (y2 - y1);

  float x_start = x2;
  float x_end = x2;

  for (int y = y2; y >= y0; y--) {
    float t1 = two_points_distance((vec4_t){x0, y0}, (vec4_t){x_start, y}) /
               two_points_distance((vec4_t){x0, y0}, (vec4_t){x2, y2});

    float i1 = lerp(intensities[0], intensities[2], t1);
    float t2 = two_points_distance((vec4_t){x1, y1}, (vec4_t){x_end, y}) /
               two_points_distance((vec4_t){x1, y1}, (vec4_t){x2, y2});

    float i2 = lerp(intensities[1], intensities[2], t2);
    int delta = x_end - x_start;
    int change = delta / abs(delta);
    for (int i = 0; i <= abs(delta) + 1; i++) {
      float t = (float)i / delta;
      float i3 = lerp(i1, i2, t);
      uint32_t new_color = light_apply_intensity(color, i3);
      draw_pixel(x_start + i * change, y, new_color);
    }
    x_start -= slope1;
    x_end -= slope2;
  }
}
