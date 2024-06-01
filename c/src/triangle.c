#include "triangle.h"
#include "display.h"

void swap(int *a, int *b) {
  int tmp = *a;
  *a = *b;
  *b = tmp;
}

void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2,
                          uint32_t color) {
  if (y0 > y1) {
    swap(&y0, &y1);
    swap(&x0, &x1);
  }
  if (y1 > y2) {
    swap(&y1, &y2);
    swap(&x1, &x2);
  }
  if (y0 > y1) {
    swap(&y0, &y1);
    swap(&x0, &x1);
  }

  if (y1 == y2) {
    fill_flat_bottom_triangle(x0, y0, x1, y1, x2, y2, color);
  } else if (y0 == y1) {
    fill_flat_top_triangle(x0, y0, x1, y1, x2, y2, color);
  } else {
    int My = y1;
    int Mx = (float)((float)((x2 - x0) * (y1 - y0)) / (float)(y2 - y0)) + x0;

    fill_flat_bottom_triangle(x0, y0, x1, y1, Mx, My, color);
    fill_flat_top_triangle(x1, y1, Mx, My, x2, y2, color);
  }
}

void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2,
                               uint32_t color) {
  float slope1 = (float)(x1 - x0) / (float)(y1 - y0);
  float slope2 = (float)(x2 - x0) / (float)(y2 - y0);
  int x_start = x0;
  int x_end = x0;
  for (int y = y0; y <= y2; y++) {
    draw_line(x_start, y, x_end, y, color);
    x_start += slope1;
    x_end += slope2;
  }
}

void fill_flat_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2,
                            uint32_t color) {
  float slope1 = (float)(x2 - x0) / (y2 - y0);
  float slope2 = (float)(x2 - x1) / (y2 - y1);
  int x_start = x2;
  int x_end = x2;
  for (int y = y2; y >= y0; y--) {
    draw_line(x_start, y, x_end, y, color);
    x_start -= slope1;
    x_end -= slope2;
  }
}
