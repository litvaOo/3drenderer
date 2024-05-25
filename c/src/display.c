#include "display.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
uint32_t *color_buffer = NULL;
int window_width;
int window_height;
SDL_Texture *color_buffer_texture = NULL;

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

  return 0;
}

void render_color_buffer(void) {
  SDL_UpdateTexture(color_buffer_texture, // texture to write to
                    NULL,                 // subsection of a texture
                    color_buffer,         // texture color buffer
                    (int)(window_width * sizeof(uint32_t)) // row size
  );
  SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
}

void clear_color_buffer(uint32_t color) {
  for (int i = 0; i < window_height * window_width; i++) {
    color_buffer[i] = color;
  }
}

void draw_grid(int multiple) {
  for (int y = 0; y < window_height; y++) {
    for (int x = 0; x < window_width; x++) {
      if (x % multiple == 0 || y % multiple == 0)
        color_buffer[window_width * y + x] = 0xFFAAAAAA;
    }
  }
}

void draw_rectangle(int x, int y, int width, int height, uint32_t color) {
  for (int new_y = y; new_y <= height + y; new_y++)
    for (int new_x = x; new_x <= width + x; new_x++)
      color_buffer[new_y * window_width + new_x] = color;
}

void destroy_window(void) {
  free(color_buffer);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}