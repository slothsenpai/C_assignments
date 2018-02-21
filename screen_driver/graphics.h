
#define RGB(r, g, b) (color_t)(((r & 31) << 11) | ((g & 63) << 5) | (b & 31))

typedef unsigned short int color_t;

void init_graphics();

void exit_graphics();

char getkey();

void sleep_ms(long ms);

void clear_screen(void *img);

void draw_pixel(void *img, int x, int y, color_t color);

void draw_line(void *img, int x1, int y1, int width, int height, color_t c);

void *new_offscreen_buffer();

void blit(void *src);
