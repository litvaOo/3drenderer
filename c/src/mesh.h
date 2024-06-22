#ifndef MESH_H
#define MESH_H

#include "texture.h"
#include "triangle.h"
#include "upng.h"
#include "vector.h"
#define N_CUBE_VERTICES 8
#define N_CUBE_FACES 12

typedef struct {
  vec3_t *vertices;
  face_t *faces;
  vec3_t *vertice_normals;
  tex2_t *texcoords;
  vec3_t rotation;
  vec3_t scale;
  vec3_t translation;
  upng_t *texture;
} mesh_t;

void load_mesh(char *obj_filename, char *png_filename, vec3_t scale,
               vec3_t translation, vec3_t rotation);
void load_mesh_obj_data(mesh_t *mesh, char *filename);
void load_mesh_png_data(mesh_t *mesh, char *filename);
int get_num_meshes();
mesh_t *get_mesh_at(int index);
#endif
