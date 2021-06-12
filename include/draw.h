/**
 * \file
 *	Drawing functions
 *
 * \author
 *	Kadir Yanık <kdrynkk@gmail.com>
 */

#ifndef DRAW_H_
#define DRAW_H_

#include <stdint.h>

/*------------------------------------------------------------------------------*/
typedef struct {
    int32_t x;
    int32_t y;
    int32_t len;
} plus_t;

typedef struct {
    int32_t x;
    int32_t y;
    int32_t width;
    int32_t height;
} rectangle_t;

typedef struct {
    int32_t x;
    int32_t y;
    int32_t r;
} circle_t;

typedef struct {
    int32_t x;
    int32_t y;
    int32_t a;
    int32_t b;
} ellipse_t;

/*------------------------------------------------------------------------------*/
void draw_plus(image_t, plus_t, uint8_t);
void draw_rect(image_t, rectangle_t, uint8_t, uint8_t);
void draw_filled_rect(image_t, rectangle_t, uint8_t, uint8_t);
void draw_circle(image_t, circle_t, uint8_t);
void draw_ellipse(image_t, ellipse_t, uint8_t);
int draw_multi_shapes(image_t, const char *, uint8_t);

#endif /* DRAW_H_ */
