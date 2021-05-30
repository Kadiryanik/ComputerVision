/**
 * \file
 *	Moment functions
 *
 * \author
 *	Kadir Yanık <kdrynkk@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "log.h"
#include "util.h"
#include "bmp.h"
#include "morphology.h"
#include "moment.h"

#ifndef LOG_LEVEL_CONF_MOMENT
#define LOG_LEVEL LOG_LEVEL_ERR
#else /* LOG_LEVEL_CONF_MOMENT */
#define LOG_LEVEL LOG_LEVEL_CONF_MOMENT
#endif /* LOG_LEVEL_CONF_MOMENT */

/*------------------------------------------------------------------------------*/
/* Currently not used */
__attribute__((unused))
static double _moment_get_variance(image_t image, region_t region)
{
    uint32_t i = 0, j = 0;
    double sum = 0, mean = 0, data_pixel_count = 0, _mean = 0, result = 0;
    rectangle_t *rect = &(region.rect);

    for (i = rect->x; i < rect->x + rect->height; i++) {
	for (j = rect->y; j < rect->y + rect->width; j++) {
	    if (image.buf[i * image.width + j] == region.label) {
		data_pixel_count++;
	    }
	}
    }

    mean = data_pixel_count / (double)(rect->width * rect->height);
    for (i = rect->x; i < rect->x + rect->height; i++) {
	for (j = rect->y; j < rect->y + rect->width; j++) {
	    if (image.buf[i * image.width + j] == region.label) {
		_mean = (1 - mean);
		sum += (_mean * _mean);
	    }
	}
    }

    result = sum / (double)((rect->width * rect->height) - 1);
    LOG_DBG("Variance for rect[%d,%d,%d,%d] = %f\n",
	    rect->x, rect->y, rect->height, rect->width, result);
    return result;
}

/*------------------------------------------------------------------------------*/
static double _moment(image_t image, region_t region, uint8_t p, uint8_t q)
{
    uint32_t i = 0, j = 0;
    double result = 0;
    rectangle_t *rect = &(region.rect);

    for (i = rect->x; i < rect->x + rect->height; i++) {
	for (j = rect->y; j < rect->y + rect->width; j++) {
	    if (image.buf[i * image.width + j] == region.label) {
		result += pow(i, p) * pow(j, q);
	    }
        }
    }
    return result;
}

/*------------------------------------------------------------------------------*/
static double _moment_central(image_t image, region_t region, uint8_t p, uint8_t q)
{
    uint32_t i = 0, j = 0;
    double result = 0, i_mean = 0, j_mean = 0, total = 0;
    rectangle_t *rect = &(region.rect);

    total = _moment(image, region, 0, 0);
    i_mean = _moment(image, region, 1, 0) / total;
    j_mean = _moment(image, region, 0, 1) / total;

    for (i = rect->x; i < rect->x + rect->height; i++) {
	for (j = rect->y; j < rect->y + rect->width; j++) {
	    if (image.buf[i * image.width + j] == region.label) {
		result += pow(i - i_mean, p) * pow(j - j_mean, q);
	    }
        }
    }
    return result;
}

/*------------------------------------------------------------------------------*/
double moment_normalized_central(image_t image, region_t region, uint8_t p, uint8_t q)
{
    double c_moment = _moment_central(image, region, p, q);
    double c_moment_zero = _moment_central(image, region, 0, 0);
    /* TODO: explain */
    int y = ((p + q) / 2) + 1;

    return c_moment / pow(c_moment_zero, y);
}

