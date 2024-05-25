#include "display.h"
#include "vector.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <stdint.h>

short is_running = -1;

#define N_POINTS (9 * 9 * 9)
vec3_t cube_points[N_POINTS];

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

void update(void) {}

void setup(void) {
  color_buffer =
      (uint32_t *)calloc(window_width * window_height, sizeof(uint32_t));
  color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                           SDL_TEXTUREACCESS_STREAMING,
                                           window_width, window_height);
  int point_count = 0;
  for (float x = -1; x <= 1; x += 0.25) {
    for (float y = -1; y <= 1; y += 0.25) {
      for (float z = -1; z <= 1; z += 0.25) {
        vec3_t new_point = {x, y, z};
        cube_points[point_count++] = new_point;
      }
    }
  }
}

void render(void) {
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0);
  SDL_RenderClear(renderer);

  draw_grid(50);

  draw_rectangle(100, 100, 100, 100, 0xFFFF0000);

  render_color_buffer();
  clear_color_buffer(0xFF000000);

  SDL_RenderPresent(renderer);
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
