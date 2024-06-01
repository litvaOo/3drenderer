#include "array.h"
#include "display.h"
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
#include <stdint.h>

short is_running = -1;

vec3_t camera_position = {0, 0, 0};
float fov_factor = 1280;

triangle_t *triangles_to_render = NULL;
int previous_frame_time = 0;

void process_input(void) {
  SDL_Event event;
  SDL_PollEvent(&event);

  switch (event.type) {
  case SDL_QUIT:
    is_running = -1;
    break;
  case SDL_KEYDOWN:
    if (event.key.keysym.sym == SDLK_ESCAPE)
      is_running = -1;
    break;
  }
}

vec2_t project(vec3_t point) {
  vec2_t projected_point = {(fov_factor * point.x) / point.z,
                            (fov_factor * point.y) / point.z};
  return projected_point;
}

void update(void) {
  int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);
  if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
    SDL_Delay(time_to_wait);

  triangles_to_render = NULL;

  mesh.rotation.y += 0.00;
  mesh.rotation.x += 0.01;
  mesh.rotation.z += 0.00;

  int num_faces = array_length(mesh.faces);
  for (int i = 0; i < num_faces; i++) {
    face_t mesh_face = mesh.faces[i];
    vec3_t face_vertices[3];
    face_vertices[0] = mesh.vertices[mesh_face.a - 1];
    face_vertices[1] = mesh.vertices[mesh_face.b - 1];
    face_vertices[2] = mesh.vertices[mesh_face.c - 1];

    triangle_t projected_triangle;

    for (int j = 0; j < 3; j++) {
      vec3_t transformed_vertex = face_vertices[j];
      vec3_rotate_x(&transformed_vertex, mesh.rotation.x);
      vec3_rotate_y(&transformed_vertex, mesh.rotation.y);
      vec3_rotate_z(&transformed_vertex, mesh.rotation.z);

      transformed_vertex.z += -5;
      face_vertices[j] = transformed_vertex;
    }

    vec3_t N = vec3_cross(vec3_sub(face_vertices[1], face_vertices[0]),
                          vec3_sub(face_vertices[2], face_vertices[0]));

    vec3_normalize(&N);
    vec3_t CR = vec3_sub(camera_position, face_vertices[0]);

    if (vec3_dot(N, CR) < 0)
      continue;

    for (int j = 0; j < 3; j++) {

      vec2_t projected_point = project(face_vertices[j]);

      projected_point.x += (window_width / 2.0);
      projected_point.y += (window_height / 2.0);
      projected_triangle.points[j] = projected_point;
    }

    array_push(triangles_to_render, projected_triangle);
  }
}

void setup(void) {
  color_buffer =
      (uint32_t *)calloc(window_width * window_height, sizeof(uint32_t));
  color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                           SDL_TEXTUREACCESS_STREAMING,
                                           window_width, window_height);
  // load_cube_mesh_data();
  load_mesh_from_file("./assets/f22.obj");
}

void render(void) {
  draw_grid(50);

  int len = array_length(triangles_to_render);

  for (int i = 0; i < len; i++) {
    triangle_t triangle = triangles_to_render[i];
    draw_filled_triangle(triangle.points[0].x, triangle.points[0].y,
                         triangle.points[1].x, triangle.points[1].y,
                         triangle.points[2].x, triangle.points[2].y,
                         0xFFFFFFFF);
    draw_triangle(triangle.points[0].x, triangle.points[0].y,
                  triangle.points[1].x, triangle.points[1].y,
                  triangle.points[2].x, triangle.points[2].y, 0xFF000000);
  }

  // draw_filled_triangle(300, 100, 50, 400, 500, 700, 0xFFFFFFFF);

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
