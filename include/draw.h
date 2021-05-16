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
void draw_plus(image_t, plus_t);
void draw_rect(image_t, rectangle_t);
void draw_circle(image_t, circle_t);
void draw_ellipse(image_t, ellipse_t);
int draw_multi_shapes(image_t, const char *);

#endif /* DRAW_H_ */
