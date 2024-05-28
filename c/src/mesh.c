#include "mesh.h"
#include "array.h"
#include "triangle.h"
#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

mesh_t mesh = {NULL, NULL, {0, 0, 0}};

vec3_t cube_vertices[N_CUBE_VERTICES] = {
    {-1, -1, -1}, {-1, 1, -1}, {1, 1, -1}, {1, -1, -1},
    {1, 1, 1},    {1, -1, 1},  {-1, 1, 1}, {-1, -1, 1},
};

face_t cube_faces[N_CUBE_FACES] = {
    // front
    {1, 2, 3},
    {1, 3, 4},
    // right
    {4, 3, 5},
    {4, 5, 6},
    // back
    {6, 5, 7},
    {6, 7, 8},
    // left
    {8, 7, 2},
    {8, 2, 1},
    // top
    {2, 7, 5},
    {2, 5, 3},
    // bottom
    {6, 8, 1},
    {6, 1, 4}};

void load_cube_mesh_data(void) {
  for (int i = 0; i < N_CUBE_VERTICES; i++) {
    vec3_t cube_vertex = cube_vertices[i];
    array_push(mesh.vertices, cube_vertex)
  }
  for (int i = 0; i < N_CUBE_FACES; i++) {
    face_t cube_face = cube_faces[i];
    array_push(mesh.faces, cube_face);
  }
}

void parse_vertice(char *line) {
  vec3_t vertex;
  char *token = strtok(line, " ");
  vertex.x = atof(token);
  token = strtok(NULL, " ");
  vertex.y = atof(token);
  token = strtok(NULL, " ");
  vertex.z = atof(token);
  array_push(mesh.vertices, vertex);
}

void parse_face(char *line) {
  face_t face;
  char *token1 = strtok(line, " ");
  char *token2 = strtok(NULL, " ");
  char *token3 = strtok(NULL, " ");
  face.c = atoi(strtok(token3, "/"));
  face.b = atoi(strtok(token2, "/"));
  face.a = atoi(strtok(token1, "/"));
  array_push(mesh.faces, face);
}

void load_mesh_from_file(char *filename) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    fprintf(stderr, "Failed loading file");
    exit(EXIT_FAILURE);
  }
  char line[255];
  while (fgets(line, 255, file)) {
    if (strlen(line) < 2)
      continue;
    if (line[0] == 'v' && line[1] == ' ')
      parse_vertice(&line[2]);
    if (line[0] == 'f')
      parse_face(&line[2]);
    continue;
  }
  fclose(file);
}
