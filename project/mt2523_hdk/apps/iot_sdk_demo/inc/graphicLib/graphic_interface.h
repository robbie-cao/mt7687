
#ifndef __GRAPHIC_INTERFACE_H__
#define __GRAPHIC_INTERFACE_H__
#if defined(_MSC_VER)
#include "gdi_type_adaptor.h"
#else
#include "stdint.h"
#endif

typedef uint32_t gdi_color;

void gdi_init(uint8_t *frame_buffer, int32_t buffer_size);

void gdi_flush_screen(void);

void gdi_get_clip(int32_t *x1, int32_t *y1, int32_t *x2, int32_t *y2);

void gdi_push_and_set_clip(int32_t x1, int32_t y1, int32_t x2, int32_t y2);

void gdi_pop_clip(void);

void gdi_draw_point(int32_t x, int32_t y, gdi_color pixel_color);

void gdi_draw_line(int32_t x1, int32_t y1, int32_t x2, int32_t y2, gdi_color line_color);

void gdi_draw_rect(int32_t x, int32_t y, int32_t width, int32_t height, gdi_color frame_color);

void gdi_draw_solid_rect(int32_t x, int32_t y, int32_t width, int32_t height, gdi_color rect_color);

//void gdi_draw_circle(int32_t x, int32_t y, int32_t r, gdi_color circle_color);

//void gdi_draw_solid_circle(int32_t x, int32_t y, int32_t r, gdi_color circle_color);

void gdi_draw_image(int32_t x, int32_t y, uint8_t* img_src);

//content of string is unicode, so wild string should be used.
void font_engine_show_string(int32_t x, int32_t y, uint8_t* string, int32_t len, gdi_color font_color);

void font_engine_get_string_width_height(uint8_t* string, int32_t len, int32_t* width_p, int32_t* height_p);

#endif

