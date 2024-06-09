#include "array.h"
#include "display.h"
#include "lighting.h"
#include "matrix.h"
#include "mesh.h"
#include "triangle.h"
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
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

short is_running = -1;

vec3_t camera_position = {0, 0, 0};

mat4_t projection_matrix;

light_t light = {{0, 0, 1}};

triangle_t *triangles_to_render = NULL;
int previous_frame_time = 0;

enum renderOption {
  WIREFRAME_DOT,
  WIREFRAME_ONLY,
  FILL_ONLY,
  WIREFRAME_FILL
} renderOption = WIREFRAME_FILL;
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
  }
}

int partition(triangle_t *triangles, int low, int high) {
  triangle_t pivot = triangles[(low + high) / 2];
  int i = low;
  int j = high;
  while (1) {
    while (triangles[i].avg_depth < pivot.avg_depth)
      i++;
    while (triangles[j].avg_depth > pivot.avg_depth)
      j--;
    if (i >= j)
      return j;
    triangle_t tmp = triangles[i];
    triangles[i] = triangles[j];
    triangles[j] = tmp;
    i++;
    j--;
  }
}

void sort_triangles(triangle_t *triangles, int low, int high) {
  if (low >= 0 && high >= 0 && low < high) {
    int mid = partition(triangles, low, high);
    sort_triangles(triangles, low, mid);
    sort_triangles(triangles, mid + 1, high);
  }
}

void update(void) {
  int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);
  if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
    SDL_Delay(time_to_wait);

  triangles_to_render = NULL;

  // mesh.rotation.y = 100.0;
  // mesh.rotation.x = 100.0;
  // mesh.rotation.z = 100.0;
  mesh.rotation.y += 0.01;
  mesh.rotation.x += 0.01;
  mesh.rotation.z += 0.01;
  //
  // mesh.scale.x += 0.002;
  // mesh.scale.y += 0.001;
  //
  // mesh.translation.x += 0.01;
  mesh.translation.z = -5;

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

      projected_points[j].x *= (window_width / 2.0);
      projected_points[j].y *= (window_height / 2.0);
      projected_points[j].x += (window_width / 2.0);
      projected_points[j].y += (window_height / 2.0);
    }

    // float light_intensity = vec3_dot(N, light.direction);
    //
    // uint32_t triangle_color =
    //     light_apply_intensity(mesh_face.color, light_intensity);
    triangle_t projected_triangle = {
        .points =
            {
                {projected_points[0].x, projected_points[0].y},
                {projected_points[1].x, projected_points[1].y},
                {projected_points[2].x, projected_points[2].y},
            },
        .color = mesh_face.color,
        .intensities = {vec3_dot(normals[0], light.direction),
                        vec3_dot(normals[1], light.direction),
                        vec3_dot(normals[2], light.direction)},
        .avg_depth =
            (face_vertices[0].z + face_vertices[1].z + face_vertices[2].z) /
            3.0};
    array_push(triangles_to_render, projected_triangle);
  }
  sort_triangles(triangles_to_render, 0, array_length(triangles_to_render) - 1);
}

void setup(void) {
  color_buffer =
      (uint32_t *)calloc(window_width * window_height, sizeof(uint32_t));
  color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                           SDL_TEXTUREACCESS_STREAMING,
                                           window_width, window_height);
  float fov = M_PI / 3.0;
  projection_matrix = mat4_make_perspective(
      fov, (float)window_height / (float)window_width, 0.1, 100.0);
  light.direction.z = 1;
  light.direction.y = 0;
  light.direction.x = 0;
  vec3_normalize(&light.direction);
  // load_cube_mesh_data();
  load_mesh_from_file("./assets/f22.obj");
  // load_mesh_from_file("./assets/cube.obj");
}

void render(void) {
  draw_grid(50);

  int len = array_length(triangles_to_render);

  for (int i = 0; i < len; i++) {
    triangle_t triangle = triangles_to_render[i];
    if (renderOption == WIREFRAME_FILL || renderOption == FILL_ONLY)
      draw_filled_triangle(triangle);
    if (renderOption != FILL_ONLY)
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

  array_free(triangles_to_render);
  render_color_buffer();
  clear_color_buffer(0xFF000000);

  SDL_RenderPresent(renderer);
}

void free_resources(void) {
  free(color_buffer);
  array_free(mesh.faces);
  array_free(mesh.vertices);
}

int main() {
  is_running = initialize_window();

  setup();

  while (is_running == 0) {
    process_input();
    update();
    render();
  }
  destroy_window();
  return 0;
}
