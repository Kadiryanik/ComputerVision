/**
 * \file
 *	Masking functions
 *
 * \author
 *	Kadir Yanık <kdrynkk@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "util.h"
#include "mask.h"

#ifndef LOG_LEVEL_CONF_MASK
#define LOG_LEVEL LOG_LEVEL_ERR
#else /* LOG_LEVEL_CONF_MASK */
#define LOG_LEVEL LOG_LEVEL_CONF_MASK
#endif /* LOG_LEVEL_CONF_MASK */

/*------------------------------------------------------------------------------*/
static int _get_sum(mask_t mask)
{
    uint32_t i = 0, j = 0;
    uint16_t sum = 0;

    for (i = 0; i < mask.height; i++) {
	for (j = 0; j < mask.width; j++) {
	    sum += mask.buf[i * mask.width + j];
	}
    }
    return sum;
}

/*------------------------------------------------------------------------------*/
mask_t* mask_read_from_file(const char *filename)
{
    FILE *file = NULL;
    mask_t *mask = NULL;
    uint32_t i = 0, j = 0;

    LOG_DBG("filename:'%s'\n", filename);

    util_fite(((file = fopen(filename, "r")) == NULL), LOG_ERR("File open failed!\n"));

    util_fite(((mask = (mask_t *)calloc(1, sizeof(mask_t))) == NULL),
	    LOG_ERR("Mask allocation failed!\n"));

    util_fite(((fscanf(file, "%u %u", &mask->width, &mask->height)) != 2),
	    LOG_ERR("Reading width/height failed!\n"));

    util_fite((mask->width == 0 || mask->height == 0),
	    LOG_ERR("width/height can not be zero!\n"));

    util_fite(((mask->buf = (int16_t *)malloc(mask->width * mask->height * sizeof(int16_t))) == NULL),
	    LOG_ERR("Mask buf allocation failed!\n"));

    for (i = 0; i < mask->height; i++) {
	for (j = 0; j < mask->width; j++) {
	    int16_t *ptr = &mask->buf[i * mask->width + j];
	    util_fite((fscanf(file, "%hd", ptr) != 1),
			LOG_ERR("Reading mask[%u][%u] failed!\n", i, j));
	    LOG_DBG_("mask[%u][%u]=%d\n", i, j, *ptr);
	}
    }
    goto success;

fail:
    sfree_mask(mask);

success:
    if (file) fclose(file);
    return mask;
}

/*------------------------------------------------------------------------------*/
int mask_apply(image_t image, mask_t mask)
{
    int ret = 0;
    uint8_t *temp_buf = NULL;
    uint32_t i = 0, j = 0, k = 0, l = 0, mask_center_i = 0, mask_center_j = 0;
    uint16_t new_val = 0, mask_divide_by = 0;

    LOG_DBG("image:%p, mask:%p\n", &image, &mask);

    util_fite(((temp_buf = (uint8_t *)malloc(image.size * sizeof(uint8_t))) == NULL),
	    LOG_ERR("Mask temp buffer allocation failed\n"));
    /* duplicate image buffer for holding original values */
    memcpy(temp_buf, image.buf, image.size);

    mask_center_i = mask.height / 2;
    mask_center_j = mask.width / 2;
    /* get and make sure it doesnt contain zero */
    mask_divide_by = _get_sum(mask);
    mask_divide_by = mask_divide_by ? mask_divide_by : 1;

    /* i and j points to the mask center */
    for (i = mask_center_i; i < image.height - mask_center_i; i++) {
	for (j = mask_center_j; j < image.width - mask_center_j; j++) {
	    new_val = 0;
	    for (k = 0; k < mask.height; k++) {
		for (l = 0; l < mask.width; l++) {
		    new_val += (mask.buf[k * mask.width + l] *
			temp_buf[(i + k - mask_center_i) * image.width + j + l - mask_center_j]);
		}
	    }
	    image.buf[i * image.width + j] = new_val / mask_divide_by;
	}
    }

    goto success;

fail:
    ret = -1;

success:
    sfree(temp_buf);
    return ret;
}

