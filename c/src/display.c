#include "display.h"
#include "array.h"
#include "mesh.h"
#include "upng.h"
#include <SDL2/SDL_stdinc.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static uint32_t *color_buffer = NULL;
static float *z_buffer = NULL;
static int window_width;
static int window_height;
static SDL_Texture *color_buffer_texture = NULL;
static int buffer_size;

int get_window_width(void) { return window_width; }
int get_window_height(void) { return window_height; }
int get_buffer_size(void) { return buffer_size; }
short initialize_window(void) {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    fprintf(stderr, "Error initializing SDL.\n");
    return -1;
  }

  SDL_DisplayMode display_mode;
  SDL_GetCurrentDisplayMode(0, &display_mode);
  window_width = display_mode.w;
  window_height = display_mode.h;
  buffer_size = window_height * window_width;
  window =
      SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       window_width, window_height, SDL_WINDOW_BORDERLESS);
  if (!window) {
    fprintf(stderr, "Error creating window,\n");
    return -1;
  }

  renderer = SDL_CreateRenderer(window, -1, 0);
  if (!renderer) {
    fprintf(stderr, "Error creating renderer,\n");
    return -1;
  }

  SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

  color_buffer = (uint32_t *)_mm_malloc(
      window_width * window_height * sizeof(uint32_t), 32);
  color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
                                           SDL_TEXTUREACCESS_STREAMING,
                                           window_width, window_height);
  z_buffer =
      (float *)_mm_malloc(sizeof(float) * window_width * window_height, 32);
  return 0;
}

void render_color_buffer(void) {
  SDL_UpdateTexture(color_buffer_texture, // texture to write to
                    NULL,                 // subsection of a texture
                    color_buffer,         // texture color buffer
                    (int)(window_width * sizeof(uint32_t)) // row size
  );
  SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
  SDL_RenderPresent(renderer);
}

float get_zbuffer_at(int x, int y) {
  if (x < 0 || x >= window_width || y < 0 || y >= window_height)
    return 1.0;
  return z_buffer[(window_width * y) + x];
}
void set_zbuffer_at(int x, int y, float value) {
  if (x < 0 || x >= window_width || y < 0 || y >= window_height)
    return;
  z_buffer[(window_width * y) + x] = value;
}

void clear_color_buffer(uint32_t color) {
  __m256i value = _mm256_set1_epi32(color);
  for (int i = 0; i < buffer_size; i += 8) {
    _mm256_store_si256((__m256i *)(color_buffer + i), value);
  }
}

void clear_z_buffer(void) {
  __m256 value = _mm256_set1_ps(1.0);
  for (int i = 0; i < buffer_size; i += 8) {
    _mm256_store_ps(z_buffer + i, value);
  }
}

void draw_line(int x0, int y0, int x1, int y1, uint32_t color) {
  int delta_x = x1 - x0;
  int delta_y = y1 - y0;
  int longest_side_length = fmax(abs(delta_x), abs(delta_y));

  float x_inc = delta_x / (float)longest_side_length;
  float y_inc = delta_y / (float)longest_side_length;

  float current_x = x0;
  float current_y = y0;

  for (int i = 0; i <= longest_side_length; i++) {
    draw_pixel(round(current_x), round(current_y), color);
    current_x += x_inc;
    current_y += y_inc;
  }
}

void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2,
                   uint32_t color) {
  draw_line(x0, y0, x1, y1, color);
  draw_line(x1, y1, x2, y2, color);
  draw_line(x2, y2, x0, y0, color);
}

void draw_rectangle(int x, int y, int width, int height, uint32_t color) {
  for (int new_y = y; new_y <= height + y; new_y++)
    for (int new_x = x; new_x <= width + x; new_x++)
      draw_pixel(new_x, new_y, color);
}

void destroy_window(void) {
  free(color_buffer);
  free(z_buffer);
  for (int i = 0; i < get_num_meshes(); i++) {
    mesh_t *mesh = get_mesh_at(i);
    array_free(mesh->faces);
    array_free(mesh->texcoords);
    array_free(mesh->vertices);
    array_free(mesh->vertice_normals);
    upng_free(mesh->texture);
  }
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void draw_pixel(int x, int y, uint32_t color) {
  if (x < 0 || x >= window_width || y < 0 || y >= window_height)
    return;
  color_buffer[y * window_width + x] = color;
}
