#include "clipping.h"
#include "texture.h"
#include "vector.h"

#define NUM_PLANES 6
plane_t frustum_planes[6];

void initialize_frustum_plane(float fovx, float fovy, float z_near,
                              float z_far) {
  float cos_half_fovx = cos(fovx / 2);
  float sin_half_fovx = sin(fovx / 2);

  float cos_half_fovy = cos(fovy / 2);
  float sin_half_fovy = sin(fovy / 2);
  vec3_t origin = {0, 0, 0};

  frustum_planes[LEFT_FRUSTUM_PLANE].point = origin;
  frustum_planes[LEFT_FRUSTUM_PLANE].normal.x = cos_half_fovx;
  frustum_planes[LEFT_FRUSTUM_PLANE].normal.y = 0;
  frustum_planes[LEFT_FRUSTUM_PLANE].normal.z = sin_half_fovx;

  frustum_planes[RIGHT_FRUSTUM_PLANE].point = origin;
  frustum_planes[RIGHT_FRUSTUM_PLANE].normal.x = -cos_half_fovx;
  frustum_planes[RIGHT_FRUSTUM_PLANE].normal.y = 0;
  frustum_planes[RIGHT_FRUSTUM_PLANE].normal.z = sin_half_fovx;

  frustum_planes[TOP_FRUSTUM_PLANE].point = origin;
  frustum_planes[TOP_FRUSTUM_PLANE].normal.x = 0;
  frustum_planes[TOP_FRUSTUM_PLANE].normal.y = -cos_half_fovy;
  frustum_planes[TOP_FRUSTUM_PLANE].normal.z = sin_half_fovy;

  frustum_planes[BOTTOM_FRUSTUM_PLANE].point = origin;
  frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.x = 0;
  frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.y = cos_half_fovy;
  frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.z = sin_half_fovy;

  frustum_planes[NEAR_FRUSTUM_PLANE].point = (vec3_t){0, 0, z_near};
  frustum_planes[NEAR_FRUSTUM_PLANE].normal.x = 0;
  frustum_planes[NEAR_FRUSTUM_PLANE].normal.y = 0;
  frustum_planes[NEAR_FRUSTUM_PLANE].normal.z = 1;

  frustum_planes[FAR_FRUSTUM_PLANE].point = (vec3_t){0, 0, z_far};
  frustum_planes[FAR_FRUSTUM_PLANE].normal.x = 0;
  frustum_planes[FAR_FRUSTUM_PLANE].normal.y = 0;
  frustum_planes[FAR_FRUSTUM_PLANE].normal.z = -1;
}

polygon_t create_polygon_from_triangle(vec3_t v0, vec3_t v1, vec3_t v2,
                                       tex2_t t0, tex2_t t1, tex2_t t2) {
  return (polygon_t){3, {v0, v1, v2}, {t0, t1, t2}};
}

float lerp(float a, float b, float t) { return a + t * (b - a); }

void clip_polygon_against_plane(polygon_t *polygon, int plane) {
  vec3_t plane_point = frustum_planes[plane].point;
  vec3_t plane_normal = frustum_planes[plane].normal;

  vec3_t inside_vertices[MAX_NUM_VERTICES];
  tex2_t inside_texcoords[MAX_NUM_VERTICES];
  int num_inside_vertices = 0;

  vec3_t *current_vertex = &polygon->vertices[0];
  tex2_t *current_texcoord = &polygon->texcoords[0];
  vec3_t *previous_vertex = &polygon->vertices[polygon->num_vertices - 1];
  tex2_t *previous_texcoord = &polygon->texcoords[polygon->num_vertices - 1];

  float current_dot = 0;
  float previous_dot =
      vec3_dot(vec3_sub(*previous_vertex, plane_point), plane_normal);

  while (current_vertex != &polygon->vertices[polygon->num_vertices]) {
    current_dot =
        vec3_dot(vec3_sub(*current_vertex, plane_point), plane_normal);

    if (current_dot * previous_dot < 0) {
      float t = previous_dot / (previous_dot - current_dot);

      vec3_t intersection_point = {
          lerp(previous_vertex->x, current_vertex->x, t),
          lerp(previous_vertex->y, current_vertex->y, t),
          lerp(previous_vertex->z, current_vertex->z, t)};
      tex2_t interpolated_texcoord = {
          lerp(previous_texcoord->u, current_texcoord->u, t),
          lerp(previous_texcoord->v, current_texcoord->v, t)};
      inside_vertices[num_inside_vertices] = vec3_clone(&intersection_point);
      inside_texcoords[num_inside_vertices] =
          tex2_clone(&interpolated_texcoord);
      num_inside_vertices++;
    }

    if (current_dot > 0) {
      inside_vertices[num_inside_vertices] = vec3_clone(current_vertex);
      inside_texcoords[num_inside_vertices] = tex2_clone(current_texcoord);
      num_inside_vertices++;
    }

    previous_dot = current_dot;
    previous_vertex = current_vertex;
    previous_texcoord = current_texcoord;
    current_vertex++;
    current_texcoord++;
  }
  for (int i = 0; i < num_inside_vertices; i++) {
    polygon->vertices[i] = vec3_clone(&inside_vertices[i]);
    polygon->texcoords[i] = tex2_clone(&inside_texcoords[i]);
  }
  polygon->num_vertices = num_inside_vertices;
}

void clip_polygon(polygon_t *polygon) {
  clip_polygon_against_plane(polygon, LEFT_FRUSTUM_PLANE);
  clip_polygon_against_plane(polygon, RIGHT_FRUSTUM_PLANE);
  clip_polygon_against_plane(polygon, TOP_FRUSTUM_PLANE);
  clip_polygon_against_plane(polygon, BOTTOM_FRUSTUM_PLANE);
  clip_polygon_against_plane(polygon, NEAR_FRUSTUM_PLANE);
  clip_polygon_against_plane(polygon, FAR_FRUSTUM_PLANE);
}

void triangles_from_polygon(polygon_t *polygon, triangle_t triangles[10],
                            int *num_triangles) {
  for (int i = 0; i < polygon->num_vertices - 2; i++) {
    triangles[i].points[0] = vec4_from_vec3(polygon->vertices[0]);
    triangles[i].points[1] = vec4_from_vec3(polygon->vertices[i + 1]);
    triangles[i].points[2] = vec4_from_vec3(polygon->vertices[i + 2]);
    triangles[i].texcoords[0] = polygon->texcoords[0];
    triangles[i].texcoords[1] = polygon->texcoords[i + 1];
    triangles[i].texcoords[2] = polygon->texcoords[i + 2];
  }
  *num_triangles = polygon->num_vertices - 2;
}
