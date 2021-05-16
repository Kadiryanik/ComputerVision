/**
 * \file
 *	Utilities
 *
 * \author
 *	Kadir Yanık <kdrynkk@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "log.h"
#include "util.h"

#ifndef LOG_LEVEL_CONF_UTIL
#define LOG_LEVEL LOG_LEVEL_ERR
#else /* LOG_LEVEL_CONF_UTIL */
#define LOG_LEVEL LOG_LEVEL_CONF_UTIL
#endif /* LOG_LEVEL_CONF_UTIL */

/*------------------------------------------------------------------------------*/
#define PI 3.141592
#define HISTOGRAM_LENGTH 256
#define HISTOGRAM_FILE_NAME "hist.txt"
#define SYS_CALL_PLOT_HISTOGRAM "python helper/plot.py "HISTOGRAM_FILE_NAME" &"

/*------------------------------------------------------------------------------*/
extern int plot_with_python; /* defined in test.c */

/*------------------------------------------------------------------------------*/
int plot_histogram(const uint32_t* const histogram)
{
    int ret = 0;
    FILE *file = NULL;

    /* Check plot is needed */
    util_sit((plot_with_python == 0));

    util_fite(((file = fopen(HISTOGRAM_FILE_NAME, "w")) == NULL),
	    LOG_ERR("File open failed\n"));

    util_fite((fwrite(histogram, HISTOGRAM_LENGTH, sizeof(int), file) < 1),
	    LOG_ERR("File writing data failed!\n"));

    util_fite((system(SYS_CALL_PLOT_HISTOGRAM) != 0),
	    LOG_ERR("CMD: %s failed!\n", SYS_CALL_PLOT_HISTOGRAM));

    goto success;

fail:
    LOG_ERR("%s failed!\n", __func__);
    ret = -1;

success:
    if (file) fclose(file);
    return ret;
}

/*------------------------------------------------------------------------------*/
/*    |	    ^
 *  --o--   x	<- y ->
 *    |	    v
 */
void draw_plus(image_t image, plus_t plus)
{
    int32_t i = 0;

    for (i = -plus.len / 2; i <= plus.len / 2; i++) {
	int32_t _x = plus.x + i;
	int32_t _y = plus.y + i;
	if (_x >= 0 && _x < image.height && plus.y >= 0 && plus.y < image.width) {
	    *(image.buf + (image.width * _x) + plus.y) = 0xff;
	}
	if (_y >= 0 && _y < image.width && plus.x >= 0 && plus.x < image.height) {
	    *(image.buf + (image.width * plus.x) + _y) = 0xff;
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
    int32_t i = 0, _x = 0, _y = 0;

    /* A to B and D to C */
    for (i = -rect.height / 2; i <= rect.height / 2; i++) {
	_x = rect.x + i;

	if (_x >= 0 && _x < image.height) {
	    /* A to B */
	    _y = rect.y - (rect.width / 2);
	    if (_y >= 0) {
		*(image.buf + (image.width * _x) + _y) = 0xff;
	    }

	    /* D to C */
	    _y = rect.y + (rect.width / 2);
	    if (_y < image.width) {
		*(image.buf + (image.width * _x) + _y) = 0xff;
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
		*(image.buf + (image.width * _x) + _y) = 0xff;
	    }

	    /* B to C */
	    _x = rect.x + (rect.height / 2);
	    if (_x < image.height) {
		*(image.buf + (image.width * _x) + _y) = 0xff;
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
    int32_t i = 0, _x = 0, _y = 0;

    for (i = 0; i < 360; i++) {
        _x = circle.r * cos((PI * i) / 180) + circle.x;
        _y = circle.r * sin((PI * i) / 180) + circle.y;

        if(_x >= 0 && _x < image.height && _y >= 0 && _y < image.width){
	    *(image.buf + (image.width * _x) + _y) = 0xff;
	}
    }
}

/*--------------------------------------------------------------------------------------*/
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
    int32_t i = 0, _x = 0, _y = 0;

    for (i = 0; i < 360; i++) {
        _x = ellipse.x + ellipse.a * cos((PI * i) / 180);
        _y = ellipse.y + ellipse.b * sin((PI * i) / 180);

        if(_x >= 0 && _x < image.height && _y >= 0 && _y < image.width){
	    *(image.buf + (image.width * _x) + _y) = 0xff;
	}
    }
}

