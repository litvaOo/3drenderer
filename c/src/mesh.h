#ifndef MESH_H
#define MESH_H

#include "texture.h"
#include "triangle.h"
#include "vector.h"
#define N_CUBE_VERTICES 8
#define N_CUBE_FACES 12

extern vec3_t cube_vertices[N_CUBE_VERTICES];
extern face_t cube_faces[N_CUBE_FACES];

typedef struct {
  vec3_t *vertices;
  face_t *faces;
  vec3_t *vertice_normals;
  tex2_t *texcoords;
  vec3_t rotation;
  vec3_t scale;
  vec3_t translation;
} mesh_t;

extern mesh_t mesh;

void load_cube_mesh_data();
void load_mesh_from_file(char *filename);
#endif
