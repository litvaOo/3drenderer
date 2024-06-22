#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include <stdint.h>

#define FPS 1000
#define FRAME_TARGET_TIME (1000 / FPS)

short initialize_window(void);

void draw_grid(int multiple);

void draw_rectangle(int x, int y, int width, int height, uint32_t color);
void draw_pixel(int x, int y, uint32_t color);
void render_color_buffer(void);
void clear_color_buffer(uint32_t color);
void destroy_window(void);
void draw_line(int x0, int y0, int x1, int y1, uint32_t color);
void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2,
                   uint32_t color);
void clear_z_buffer(void);
int get_window_width(void);
int get_window_height(void);
int get_buffer_size(void);
float get_zbuffer_at(int x, int y);
void set_zbuffer_at(int x, int y, float value);
#endif
