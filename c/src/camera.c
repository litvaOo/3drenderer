#include "camera.h"
#include "matrix.h"
#include "vector.h"

mat4_t look_at(vec3_t eye, vec3_t target, vec3_t up) {
  vec3_t z = vec3_sub(target, eye);
  vec3_normalize(&z);
  vec3_t x = vec3_cross(up, z);
  vec3_normalize(&x);
  vec3_t y = vec3_cross(z, x);
  mat4_t view_matrix = {{{x.x, x.y, x.z, -vec3_dot(x, eye)},
                         {y.x, y.y, y.z, -vec3_dot(y, eye)},
                         {z.x, z.y, z.z, -vec3_dot(z, eye)},
                         {0, 0, 0, 1}}};
  return view_matrix;
}
