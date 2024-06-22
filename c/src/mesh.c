#include "mesh.h"
#include "array.h"
#include "texture.h"
#include "triangle.h"
#include "upng.h"
#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NUM_MESHES 10
static mesh_t meshes[MAX_NUM_MESHES];
static int mesh_count = 0;

void load_mesh(char *obj_filename, char *png_filename, vec3_t scale,
               vec3_t translation, vec3_t rotation) {
  load_mesh_obj_data(&meshes[mesh_count], obj_filename);
  load_mesh_png_data(&meshes[mesh_count], png_filename);
  meshes[mesh_count].scale = scale;
  meshes[mesh_count].translation = translation;
  meshes[mesh_count].rotation = rotation;
  mesh_count++;
}

void load_mesh_png_data(mesh_t *mesh, char *filename) {
  upng_t *png_texture = upng_new_from_file(filename);
  if (png_texture != NULL) {
    upng_decode(png_texture);
    if (upng_get_error(png_texture) == UPNG_EOK) {
      mesh->texture = png_texture;
    }
  }
}

int get_num_meshes() { return mesh_count; }
mesh_t *get_mesh_at(int index) { return &meshes[index]; }

void load_mesh_obj_data(mesh_t *mesh, char *filename) {
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
        array_push(mesh->vertices, vertex);
      }
      if (line[1] == 'n') {
        vec3_t normal;
        sscanf(&line[2], "%f %f %f", &normal.x, &normal.y, &normal.z);
        array_push(mesh->vertice_normals, normal);
      }
      if (line[1] == 't') {
        tex2_t texcoord;
        sscanf(&line[2], "%f %f", &texcoord.u, &texcoord.v);
        array_push(mesh->texcoords, texcoord);
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
          vertex_indices[0],
          vertex_indices[1],
          vertex_indices[2],
          0xFFFF0000,
          .texcoords = {texture_indices[0], texture_indices[1],
                        texture_indices[2]},
          .normals = {normal_indices[0], normal_indices[1], normal_indices[2]}};
      array_push(mesh->faces, face);
    }
    continue;
  }
  fclose(file);
}
