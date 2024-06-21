#ifndef CAMERA_H
#define CAMERA_H

#include "matrix.h"
#include "vector.h"
typedef struct {
  vec3_t position;
  vec3_t direction;
  vec3_t forward_velocity;
  float yaw_angle;
} camera_t;

mat4_t look_at(vec3_t eye, vec3_t target, vec3_t up);

#endif
