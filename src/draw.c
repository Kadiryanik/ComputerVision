/**
 * \file
 *	Drawing functions
 *
 * \author
 *	Kadir Yanık <kdrynkk@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "log.h"
#include "util.h"
#include "bmp.h"
#include "draw.h"

#ifndef LOG_LEVEL_CONF_DRAW
#define LOG_LEVEL LOG_LEVEL_ERR
#else /* LOG_LEVEL_CONF_DRAW */
#define LOG_LEVEL LOG_LEVEL_CONF_DRAW
#endif /* LOG_LEVEL_CONF_DRAW */

/*------------------------------------------------------------------------------*/
#define PI 3.141592
#define SHAPE_NAME_LEN 16
#define SHAPE_NAME_SF "%15s" /* SF: Scanf Format */

#define DRAW_RGB_COLOUR		(uint8_t []){ 0, 255, 255 } /* R: 0, G: 255, B: 255 */
#define DRAW_GRAYSCLE_COLOUR	COLOUR_WHITE
#define GET_COLOUR(n, k)	((n > 1) ? DRAW_RGB_COLOUR[k] : DRAW_GRAYSCLE_COLOUR)

#define draw_set_pixels(_ptr) do {			\
	ptr = _ptr;					\
	for (k = 0; k < image.cb; k++) {		\
	    *(ptr + k) = GET_COLOUR(image.cb, k);	\
	}						\
    } while (0)

/*------------------------------------------------------------------------------*/
/*    |	    ^
 *  --o--   x	<- y ->
 *    |	    v
 */
void draw_plus(image_t image, plus_t plus)
{
    int32_t i = 0, k = 0;
    uint8_t *ptr = NULL;

    LOG_DBG("plus %d %d %d\n", plus.x, plus.y, plus.len);

    for (i = -plus.len / 2; i <= plus.len / 2; i++) {
	int32_t _x = plus.x + i;
	int32_t _y = plus.y + i;
	if (_x >= 0 && _x < image.height && plus.y >= 0 && plus.y < image.width) {
	    draw_set_pixels(image.buf + (image.width * image.cb * _x) +
		    plus.y * image.cb);
	}
	if (_y >= 0 && _y < image.width && plus.x >= 0 && plus.x < image.height) {
	    draw_set_pixels(image.buf + (image.width * image.cb * plus.x) +
		   _y * image.cb);
	}
    }
}

/*------------------------------------------------------------------------------*/
/* A--------------D
 * |      | h/2   | ^
 * |------o       | x	<- y ->
 * | w/2          | v
 * B--------------C
 */
void draw_rect(image_t image, rectangle_t rect)
{
    int32_t i = 0, k = 0, _x = 0, _y = 0;
    uint8_t *ptr = NULL;

    LOG_DBG("rectangle %d %d %d %d\n", rect.x, rect.y, rect.width, rect.height);
    /* A to B and D to C */
    for (i = -rect.height / 2; i <= rect.height / 2; i++) {
	_x = rect.x + i;

	if (_x >= 0 && _x < image.height) {
	    /* A to B */
	    _y = rect.y - (rect.width / 2);
	    if (_y >= 0) {
		draw_set_pixels(image.buf + (image.width * image.cb * _x) +
			_y * image.cb);
	    }

	    /* D to C */
	    _y = rect.y + (rect.width / 2);
	    if (_y < image.width) {
		draw_set_pixels(image.buf + (image.width * image.cb * _x) +
			_y * image.cb);
	    }
	}
    }
    /* A to D and B to C */
    for (i = -rect.width / 2; i <= rect.width / 2; i++) {
	_y = rect.y + i;

	if (_y >= 0 && _y < image.width) {
	    /* A to D */
	    _x = rect.x - (rect.height / 2);
	    if (_x >= 0) {
		draw_set_pixels(image.buf + (image.width * image.cb * _x) +
			_y * image.cb);
	    }

	    /* B to C */
	    _x = rect.x + (rect.height / 2);
	    if (_x < image.height) {
		draw_set_pixels(image.buf + (image.width * image.cb * _x) +
			_y * image.cb);
	    }
	}
    }
}

/*------------------------------------------------------------------------------*/
/*      ===
 *   --     --
 *  /         \  ^
 * |     o-----| x  <- y ->
 *  \       r /  v
 *   --     --
 *      ===
 */
void draw_circle(image_t image, circle_t circle)
{
    int32_t i = 0, k = 0, _x = 0, _y = 0;
    uint8_t *ptr = NULL;

    LOG_DBG("circle %d %d %d\n", circle.x, circle.y, circle.r);

    for (i = 0; i < 360; i++) {
	_x = circle.r * cos((PI * i) / 180) + circle.x;
	_y = circle.r * sin((PI * i) / 180) + circle.y;

	if(_x >= 0 && _x < image.height && _y >= 0 && _y < image.width){
	    draw_set_pixels(image.buf + (image.width * image.cb * _x) + _y * image.cb);
	}
    }
}

/*------------------------------------------------------------------------------*/
/*      ===
 *     / | \
 *   -- a|  --
 *  /    |    \  ^
 * |     o-----| x  <- y ->
 *  \       b /  v
 *   --     --
 *     \   /
 *      ===
 */
void draw_ellipse(image_t image, ellipse_t ellipse)
{
    int32_t i = 0, k = 0, _x = 0, _y = 0;
    uint8_t *ptr = NULL;

    LOG_DBG("ellipse %d %d %d %d\n", ellipse.x, ellipse.y, ellipse.a, ellipse.b);

    for (i = 0; i < 360; i++) {
	_x = ellipse.x + ellipse.a * cos((PI * i) / 180);
	_y = ellipse.y + ellipse.b * sin((PI * i) / 180);

	if (_x >= 0 && _x < image.height && _y >= 0 && _y < image.width) {
	    draw_set_pixels(image.buf + (image.width * image.cb * _x) + _y * image.cb);
	}
    }
}

/*------------------------------------------------------------------------------*/
int draw_multi_shapes(image_t image, const char *filename)
{
    int ret = 0;
    FILE *file = NULL;
    char shape_name[SHAPE_NAME_LEN];
    plus_t plus;
    rectangle_t rect;
    circle_t circle;
    ellipse_t ellipse;

    LOG_DBG("image:%p filename:'%s'\n", &image, filename);

    util_fite(((file = fopen(filename, "r")) == NULL), LOG_ERR("File open failed!\n"));

    while (1) {
	util_fite(((fscanf(file, SHAPE_NAME_SF, shape_name)) != 1),
		LOG_ERR("Reading shape name failed!\n"));
	if (!strcmp("EOF", shape_name)) {
	    /* If we reach here, all the figures have been read properly.
	     * We can break the loop. */
	    break;
	} else if (!strcmp("plus", shape_name)) {
	    util_fite(((fscanf(file, "%d %d %d", &plus.x, &plus.y, &plus.len)) != 3),
		    LOG_ERR("%s reading failed!\n", shape_name));

	    draw_plus(image, plus);
	} else if (!strcmp("rectangle", shape_name)) {
	    util_fite(((fscanf(file, "%d %d %d %d", &rect.x, &rect.y, &rect.width,
				&rect.height)) != 4),
		    LOG_ERR("%s reading failed!\n", shape_name));

	    draw_rect(image, rect);
	} else if (!strcmp("circle", shape_name)) {
	    util_fite(((fscanf(file, "%d %d %d", &circle.x, &circle.y, &circle.r)) != 3),
		    LOG_ERR("%s reading failed!\n", shape_name));

	    draw_circle(image, circle);
	} else if (!strcmp("ellipse", shape_name)) {
	    util_fite(((fscanf(file, "%d %d %d %d", &ellipse.x, &ellipse.y,
				&ellipse.a, &ellipse.b)) != 4),
		    LOG_ERR("%s reading failed!\n", shape_name));

	    draw_ellipse(image, ellipse);
	} else {
	    LOG_ERR("Shape '%s' is not supported!\n", shape_name);
	    goto fail;
	}
    }

    LOG_DBG("All shapes successfully drawed!\n");
    goto success;

fail:
    ret = -1;

success:
    if (file) fclose(file);
    return ret;
}

