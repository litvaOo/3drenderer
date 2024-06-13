#include "mesh.h"
#include "array.h"
#include "triangle.h"
#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

mesh_t mesh = {NULL, NULL, NULL, {0, 0, 0}, {1.0, 1.0, 1.0}, {0, 0, 0}};

vec3_t cube_vertices[N_CUBE_VERTICES] = {
    {-1, -1, 1}, {1, -1, 1}, {-1, 1, 1},   {1, 1, 1},
    {-1, 1, -1}, {1, 1, -1}, {-1, -1, -1}, {1, -1, -1},
};

face_t cube_faces[N_CUBE_FACES] = {
    // front
    {1, 2, 3, 0xFFFF0000, .normals = {1, 1, 1}, .a_uv = {1, 0}, .b_uv = {0, 0},
     .c_uv = {1, 1}},
    {3, 2, 4, 0xFFFF0000, .normals = {1, 1, 1}, .a_uv = {1, 1}, .b_uv = {0, 0},
     .c_uv = {0, 1}},
    // right
    {3, 4, 5, 0xFF00FF00, .normals = {2, 2, 2}, .a_uv = {1, 0}, .b_uv = {0, 0},
     .c_uv = {1, 1}},
    {5, 4, 6, 0xFF00FF00, .normals = {2, 2, 2}, .a_uv = {1, 1}, .b_uv = {0, 0},
     .c_uv = {0, 1}},
    // back
    {5, 6, 7, 0xFF0000FF, .normals = {3, 3, 3}, .a_uv = {0, 1}, .b_uv = {1, 1},
     .c_uv = {0, 0}},
    {7, 6, 8, 0xFF0000FF, .normals = {3, 3, 3}, .a_uv = {0, 0}, .b_uv = {1, 1},
     .c_uv = {1, 0}},
    // left
    {7, 8, 1, 0xFFFFFF00, .normals = {4, 4, 4}, .a_uv = {1, 0}, .b_uv = {0, 0},
     .c_uv = {1, 1}},
    {1, 8, 2, 0xFFFFFF00, .normals = {4, 4, 4}, .a_uv = {1, 1}, .b_uv = {0, 0},
     .c_uv = {0, 1}},
    // top
    {2, 8, 4, 0xFFFF00FF, .normals = {5, 5, 5}, .a_uv = {1, 0}, .b_uv = {0, 0},
     .c_uv = {1, 1}},
    {4, 8, 6, 0xFFFF00FF, .normals = {5, 5, 5}, .a_uv = {1, 1}, .b_uv = {0, 0},
     .c_uv = {0, 1}},
    // bottom
    {7, 1, 5, 0xFF00FFFF, .normals = {6, 6, 6}, .a_uv = {1, 0}, .b_uv = {0, 0},
     .c_uv = {1, 1}},
    {5, 1, 3, 0xFF00FFFF, .normals = {6, 6, 6}, .a_uv = {1, 1}, .b_uv = {0, 0},
     .c_uv = {0, 1}}};

vec3_t cube_normals[6] = {
    {0.000000, 0.000000, 1.000000},  {0.000000, 1.000000, 0.000000},
    {0.000000, 0.000000, -1.000000}, {0.000000, -1.000000, 0.000000},
    {1.000000, 0.000000, 0.000000},  {-1.000000, 0.000000, 0.000000}};

void load_cube_mesh_data(void) {
  for (int i = 0; i < N_CUBE_VERTICES; i++) {
    vec3_t cube_vertex = cube_vertices[i];
    array_push(mesh.vertices, cube_vertex)
  }
  for (int i = 0; i < N_CUBE_FACES; i++) {
    face_t cube_face = cube_faces[i];
    array_push(mesh.faces, cube_face);
  }
  for (int i = 0; i < 6; i++) {
    vec3_t cube_normal = cube_normals[i];
    array_push(mesh.vertice_normals, cube_normal);
  }
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
    if (line[0] == 'v') {
      if (line[1] == ' ') {
        vec3_t vertex;
        sscanf(&line[2], "%f %f %f", &vertex.x, &vertex.y, &vertex.z);
        array_push(mesh.vertices, vertex);
      }
      if (line[1] == 'n') {
        vec3_t normal;
        sscanf(&line[2], "%f %f %f", &normal.x, &normal.y, &normal.z);
        array_push(mesh.vertice_normals, normal);
      }
    }
    if (line[0] == 'f') {
      int vertex_indices[3];
      int texture_indices[3];
      int normal_indices[3];
      sscanf(&line[2], "%d/%d/%d %d/%d/%d %d/%d/%d", &vertex_indices[0],
             &texture_indices[0], &normal_indices[0], &vertex_indices[1],
             &texture_indices[1], &normal_indices[1], &vertex_indices[2],
             &texture_indices[2], &normal_indices[2]);
      face_t face = {
          vertex_indices[0], vertex_indices[1], vertex_indices[2], 0xFFFF0000,
          .normals = {normal_indices[0], normal_indices[1], normal_indices[2]}};
      array_push(mesh.faces, face);
    }
    continue;
  }
  fclose(file);
}
