#include "array.h"
#include "display.h"
#include "lighting.h"
#include "matrix.h"
#include "mesh.h"
#include "texture.h"
#include "triangle.h"
#include "upng.h"
#include "vector.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

short is_running = -1;

vec3_t camera_position = {0, 0, 0};

mat4_t projection_matrix;

light_t light = {{0, 0, 1}};

#define MAX_TRIANGLES_PER_MESH 10000
triangle_t triangles_to_render[MAX_TRIANGLES_PER_MESH];

int num_triangles_to_render = 0;

int previous_frame_time = 0;

enum renderOption {
  WIREFRAME_DOT,
  WIREFRAME_ONLY,
  FILL_ONLY,
  WIREFRAME_FILL,
  TEXTURED_ONLY,
  WIREFRAME_TEXTURED
} renderOption = WIREFRAME_TEXTURED;
int backface_culling = 1;

void process_input(void) {
  SDL_Event event;
  SDL_PollEvent(&event);

  switch (event.type) {
  case SDL_QUIT:
    is_running = -1;
    break;
  case SDL_KEYDOWN:
    if (event.key.keysym.sym == SDLK_ESCAPE) {
      is_running = -1;
      break;
    }
    if (event.key.keysym.scancode == 6) {
      backface_culling = 1;
      break;
    }
    if (event.key.keysym.scancode == 7) {
      backface_culling = 0;
      break;
    }
    if (event.key.keysym.scancode == 30) {
      renderOption = WIREFRAME_DOT;
      break;
    }
    if (event.key.keysym.scancode == 31) {
      renderOption = WIREFRAME_ONLY;
      break;
    }
    if (event.key.keysym.scancode == 32) {
      renderOption = FILL_ONLY;
      break;
    }
    if (event.key.keysym.scancode == 33) {
      renderOption = WIREFRAME_FILL;
      break;
    }
    if (event.key.keysym.scancode == 34) {
      renderOption = TEXTURED_ONLY;
      break;
    }
    if (event.key.keysym.scancode == 35) {
      renderOption = WIREFRAME_TEXTURED;
      break;
    }
  }
}

void update(void) {
  int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);
  if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
    SDL_Delay(time_to_wait);

  num_triangles_to_render = 0;
  // mesh.rotation.y = 100.0;
  // mesh.rotation.x = 100.0;
  // mesh.rotation.z = 100.0;
  mesh.rotation.y += 0.02;
  mesh.rotation.x += 0.02;
  // mesh.rotation.z += 0.02;
  //
  // mesh.scale.x += 0.002;
  // mesh.scale.y += 0.001;
  //
  // mesh.translation.x += 0.01;
  mesh.translation.z = 5;

  mat4_t scale_matrix =
      mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);

  mat4_t translation_matrix = mat4_make_translation(
      mesh.translation.x, mesh.translation.y, mesh.translation.z);

  mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh.rotation.x);
  mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh.rotation.y);
  mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh.rotation.z);

  int num_faces = array_length(mesh.faces);
  for (int i = 0; i < num_faces; i++) {
    face_t mesh_face = mesh.faces[i];
    vec3_t face_vertices[3];
    face_vertices[0] = mesh.vertices[mesh_face.a - 1];
    face_vertices[1] = mesh.vertices[mesh_face.b - 1];
    face_vertices[2] = mesh.vertices[mesh_face.c - 1];
    vec3_t normals[3];
    normals[0] = mesh.vertice_normals[mesh_face.normals[0] - 1];
    normals[1] = mesh.vertice_normals[mesh_face.normals[1] - 1];
    normals[2] = mesh.vertice_normals[mesh_face.normals[2] - 1];
    for (int j = 0; j < 3; j++) {
      vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

      mat4_t world_matrix = mat4_identity();
      world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
      world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
      world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
      world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
      world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);
      transformed_vertex = mat4_mul_vec4(world_matrix, transformed_vertex);

      face_vertices[j] = vec3_from_vec4(transformed_vertex);
      mat4_t rotation_matrix = mat4_identity();
      rotation_matrix = mat4_mul_mat4(rotation_matrix_z, rotation_matrix);
      rotation_matrix = mat4_mul_mat4(rotation_matrix_y, rotation_matrix);
      rotation_matrix = mat4_mul_mat4(rotation_matrix_x, rotation_matrix);
      normals[j] = vec3_from_vec4(
          mat4_mul_vec4(rotation_matrix, vec4_from_vec3(normals[j])));
    }

    vec3_t N = vec3_cross(vec3_sub(face_vertices[1], face_vertices[0]),
                          vec3_sub(face_vertices[2], face_vertices[0]));

    vec3_normalize(&N);
    vec3_t CR = vec3_sub(camera_position, face_vertices[0]);

    if (backface_culling == 1 && vec3_dot(N, CR) < 0)
      continue;

    vec4_t projected_points[3];
    for (int j = 0; j < 3; j++) {
      projected_points[j] = mat4_mul_vec4_project(
          projection_matrix, vec4_from_vec3(face_vertices[j]));

      projected_points[j].y *= -1;

      projected_points[j].x *= (window_width / 2.0);
      projected_points[j].y *= (window_height / 2.0);

      projected_points[j].x += (window_width / 2.0);
      projected_points[j].y += (window_height / 2.0);
    }

    triangle_t projected_triangle = {
        .points =
            {
                projected_points[0],
                projected_points[1],
                projected_points[2],
            },

        .texcoords = {mesh.texcoords[mesh_face.texcoords[0] - 1],
                      mesh.texcoords[mesh_face.texcoords[1] - 1],
                      mesh.texcoords[mesh_face.texcoords[2] - 1]},
        .color = mesh_face.color,
        .intensities = {vec3_dot(normals[0], light.direction),
                        vec3_dot(normals[1], light.direction),
                        vec3_dot(normals[2], light.direction)},
    };
    if (num_triangles_to_render < MAX_TRIANGLES_PER_MESH) {
      triangles_to_render[num_triangles_to_render++] = projected_triangle;
    }
  }
}

void setup(char *obj_file, char *png_file) {
  color_buffer =
      (uint32_t *)calloc(window_width * window_height, sizeof(uint32_t));
  color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
                                           SDL_TEXTUREACCESS_STREAMING,
                                           window_width, window_height);
  z_buffer = (float *)malloc(sizeof(float) * window_width * window_height);
  float fov = M_PI / 3.0;
  projection_matrix = mat4_make_perspective(
      fov, (float)window_height / (float)window_width, 0.1, 100.0);
  light.direction.z = -1;
  light.direction.y = 0;
  light.direction.x = 0;
  vec3_normalize(&light.direction);

  load_png_texture_data(png_file);
  load_mesh_from_file(obj_file);
}

void render(void) {
  draw_grid(50);

  for (int i = 0; i < num_triangles_to_render; i++) {
    triangle_t triangle = triangles_to_render[i];
    if (renderOption == WIREFRAME_FILL || renderOption == FILL_ONLY)
      draw_textured_triangle(triangle, NULL);
    if (renderOption == TEXTURED_ONLY || renderOption == WIREFRAME_TEXTURED) {
      draw_textured_triangle(triangle, mesh_texture);
    }
    if (renderOption != FILL_ONLY && renderOption != TEXTURED_ONLY)
      draw_triangle(triangle.points[0].x, triangle.points[0].y,
                    triangle.points[1].x, triangle.points[1].y,
                    triangle.points[2].x, triangle.points[2].y, 0x696969);
    if (renderOption == WIREFRAME_DOT) {
      draw_rectangle(triangle.points[0].x - 1, triangle.points[0].y - 1, 3, 3,
                     0xFFFF0000);
      draw_rectangle(triangle.points[1].x - 1, triangle.points[1].y - 1, 3, 3,
                     0xFFFF0000);
      draw_rectangle(triangle.points[2].x - 1, triangle.points[2].y - 1, 3, 3,
                     0xFFFF0000);
    }
  }

  render_color_buffer();
  clear_color_buffer(0xFF000000);
  clear_z_buffer();

  SDL_RenderPresent(renderer);
}

void free_resources(void) {
  free(color_buffer);
  free(z_buffer);
  upng_free(png_texture);
  array_free(mesh.faces);
  array_free(mesh.vertices);
}

int main(int argc, char **argv) {
  is_running = initialize_window();

  char *obj_file;
  char *png_file;

  if (argc == 1) {
    obj_file = "./assets/f22.obj";
    png_file = "./assets/f22.png";
  } else {
    obj_file = argv[1];
    png_file = argv[2];
  }

  setup(obj_file, png_file);

  int start_time = SDL_GetTicks();
  int frames = 0;
  while (is_running == 0) {
    process_input();
    update();
    render();
    frames++;
    if (SDL_GetTicks() - start_time > 10000) {
      break;
    }
  }
  printf("Frames: %d\n", frames);
  printf("Average FPS: %f\n",
         (float)frames / ((SDL_GetTicks() - start_time) / 1000.0));
  destroy_window();
  return 0;
}
