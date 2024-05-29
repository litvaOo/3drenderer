#include "vector.h"
#include <math.h>

vec3_t vec3_rotate_x(vec3_t v, float angle) {
  vec3_t rotated_vector = {v.x, v.y * cos(angle) - v.z * sin(angle),
                           v.z * cos(angle) + v.y * sin(angle)};
  return rotated_vector;
}

vec3_t vec3_rotate_y(vec3_t v, float angle) {
  vec3_t rotated_vector = {v.x * cos(angle) - v.z * sin(angle), v.y,
                           v.z * cos(angle) + v.x * sin(angle)};
  return rotated_vector;
}

vec3_t vec3_rotate_z(vec3_t v, float angle) {
  vec3_t rotated_vector = {v.x * cos(angle) - v.y * sin(angle),
                           v.y * cos(angle) + v.x * sin(angle), v.z};
  return rotated_vector;
}

float vec2_length(vec2_t v) { return sqrtf(v.x * v.x + v.y * v.y); }

float vec3_length(vec3_t v) { return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z); }

vec3_t vec3_add(vec3_t a, vec3_t b) {
  return (vec3_t){a.x + b.x, a.y + b.y, a.z + b.z};
}

vec2_t vec2_add(vec2_t a, vec2_t b) { return (vec2_t){a.x + b.x, a.y + b.y}; }

vec3_t vec3_sub(vec3_t a, vec3_t b) {
  return (vec3_t){a.x - b.x, a.y - b.y, a.z - b.z};
}

vec2_t vec2_sub(vec2_t a, vec2_t b) { return (vec2_t){a.x - b.x, a.y - b.y}; }

vec3_t vec3_div(vec3_t v, float factor) {
  return (vec3_t){v.x / factor, v.y / factor, v.z / factor};
}

vec2_t vec2_div(vec2_t v, float factor) {
  return (vec2_t){v.x / factor, v.y / factor};
}

vec3_t vec3_mult(vec3_t v, float factor) {
  return (vec3_t){v.x * factor, v.y * factor, v.z * factor};
}

vec2_t vec2_mult(vec2_t v, float factor) {
  return (vec2_t){v.x * factor, v.y * factor};
}

vec3_t vec3_cross(vec3_t a, vec3_t b) {
  return (vec3_t){a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z,
                  a.x * b.y - a.y * b.x};
}

float vec3_dot(vec3_t a, vec3_t b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
