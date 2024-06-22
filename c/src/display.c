#include "display.h"
#include "texture.h"
#include "upng.h"
#include <math.h>
#include <stdint.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static uint32_t *color_buffer = NULL;
static float *z_buffer = NULL;
static int window_width = 320;
static int window_height = 200;
static SDL_Texture *color_buffer_texture = NULL;

int get_window_width(void) { return window_width; }
int get_window_height(void) { return window_height; }

short initialize_window(void) {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    fprintf(stderr, "Error initializing SDL.\n");
    return -1;
  }

  SDL_DisplayMode display_mode;
  SDL_GetCurrentDisplayMode(0, &display_mode);
  window_width = display_mode.w;
  window_height = display_mode.h;
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

  color_buffer =
      (uint32_t *)calloc(window_width * window_height, sizeof(uint32_t));
  color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
                                           SDL_TEXTUREACCESS_STREAMING,
                                           window_width, window_height);
  z_buffer = (float *)malloc(sizeof(float) * window_width * window_height);
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
  for (int i = 0; i < window_height * window_width; i++) {
    color_buffer[i] = color;
  }
}

void clear_z_buffer(void) {
  for (int i = 0; i < window_width * window_height; i++)
    z_buffer[i] = 1.0;
}

void draw_grid(int multiple) {
  for (int y = 0; y < window_height; y += multiple)
    for (int x = 0; x < window_width; x += multiple)
      color_buffer[window_width * y + x] = 0xFFAAAAAA;
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
  upng_free(png_texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void draw_pixel(int x, int y, uint32_t color) {
  if (x < 0 || x >= window_width || y < 0 || y >= window_height)
    return;
  color_buffer[y * window_width + x] = color;
}
