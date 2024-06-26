#ifndef CLIPPING_H
#define CLIPPING_H
#include "texture.h"
#include "triangle.h"
#include "vector.h"

enum {
  LEFT_FRUSTUM_PLANE,
  RIGHT_FRUSTUM_PLANE,
  BOTTOM_FRUSTUM_PLANE,
  TOP_FRUSTUM_PLANE,
  NEAR_FRUSTUM_PLANE,
  FAR_FRUSTUM_PLANE,
};

typedef struct {
  vec3_t point;
  vec3_t normal;
} plane_t;

#define MAX_NUM_VERTICES 10
#define MAX_NUM_TRIANGLES 10
typedef struct {
  int num_vertices;
  vec3_t vertices[MAX_NUM_VERTICES];
  tex2_t texcoords[MAX_NUM_TRIANGLES];
} polygon_t;

void initialize_frustum_plane(float fovx, float fovy, float z_near,
                              float z_far);
polygon_t create_polygon_from_triangle(vec3_t v0, vec3_t v1, vec3_t v2,
                                       tex2_t t0, tex2_t t1, tex2_t t2);
void clip_polygon(polygon_t *polygon_t);
void triangles_from_polygon(polygon_t *polygon_t, triangle_t triangles[10],
                            int *num_triangles);
#endif // !CLIPPING_H
