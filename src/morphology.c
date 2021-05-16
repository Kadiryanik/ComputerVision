/**
 * \file
 *	Morphology functions
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
#include "bmp.h"
#include "morphology.h"

#ifndef LOG_LEVEL_CONF_MORPHOLOGY
#define LOG_LEVEL LOG_LEVEL_ERR
#else /* LOG_LEVEL_CONF_MORPHOLOGY */
#define LOG_LEVEL LOG_LEVEL_CONF_MORPHOLOGY
#endif /* LOG_LEVEL_CONF_MORPHOLOGY */

/*------------------------------------------------------------------------------*/
static int _morp_apply(image_t image, mask_t mask, uint8_t check_value)
{
    int ret = 0;
    uint8_t *temp_buf = NULL;
    uint32_t i = 0, j = 0, k = 0, l = 0, mask_center_i = 0, mask_center_j = 0;

    LOG_DBG("%s: image:'%p' mask:'%p', check_val:%u\n", __func__, &image, &mask, check_value);

    util_fite(((temp_buf = (uint8_t *)malloc(image.size * sizeof(uint8_t))) == NULL),
	    LOG_ERR("Temp buffer allocation failed\n"));
    /* duplicate image buffer for holding original values */
    memcpy(temp_buf, image.buf, image.size);

    mask_center_i = mask.height / 2;
    mask_center_j = mask.width / 2;
    /* Do not proceed the border pixels */
    for (i = mask_center_i; i < image.height - mask_center_i; i++) {
	for (j = mask_center_j; j < image.width - mask_center_j; j++) {
	    /* If pixel in center of mask is equal to check_value then skip */
	    if (temp_buf[(i + mask_center_i) * image.width +
		    (j + mask_center_j)] == check_value) continue;

	    for (k = 0; k < mask.height; k++) {
		for (l = 0; l < mask.width; l++) {
		    image.buf[(i + k - mask_center_i) * image.width +
			(j + l - mask_center_j)] = mask.buf[k * mask.width + l];
		}
	    }
	}
    }

    goto success;

fail:
    ret = -1;

success:
    sfree(temp_buf);
    return ret;
}

/*------------------------------------------------------------------------------*/
/* A + B */
int morp_apply_dilation(image_t image)
{
    int ret = 0;
    mask_t mask = { .buf = (int16_t []){ COLOUR_BLACK, COLOUR_BLACK, COLOUR_BLACK,
	COLOUR_BLACK, COLOUR_BLACK, COLOUR_BLACK, COLOUR_BLACK, COLOUR_BLACK,
	COLOUR_BLACK }, .width = 3, .height = 3 };

    util_fit((_morp_apply(image, mask, COLOUR_WHITE) != 0));

    goto success;

fail:
    ret = -1;

success:
    return ret;
}

/*------------------------------------------------------------------------------*/
/* A - B */
int morp_apply_erosion(image_t image)
{
    int ret = 0;
    mask_t mask = { .buf = (int16_t []){ COLOUR_WHITE, COLOUR_WHITE, COLOUR_WHITE,
	COLOUR_WHITE, COLOUR_WHITE, COLOUR_WHITE, COLOUR_WHITE, COLOUR_WHITE,
	COLOUR_WHITE }, .width = 3, .height = 3 };

    util_fit((_morp_apply(image, mask, COLOUR_BLACK) != 0));

    goto success;

fail:
    ret = -1;

success:
    return ret;
}

/*------------------------------------------------------------------------------*/
/* (A - B) + B */
int morp_apply_open(image_t image)
{
    int ret = 0;

    util_fit((morp_apply_erosion(image) != 0));
    util_fit((morp_apply_dilation(image) != 0));

    goto success;

fail:
    ret = -1;

success:
    return ret;
}

/*------------------------------------------------------------------------------*/
/* (A + B) - B */
int morp_apply_close(image_t image)
{
    int ret = 0;

    util_fit((morp_apply_dilation(image) != 0));
    util_fit((morp_apply_erosion(image) != 0));

    goto success;

fail:
    ret = -1;

success:
    return ret;
}

/*------------------------------------------------------------------------------*/
int morp_apply(image_t image, const char *morp)
{
    int ret = 0;

    LOG_DBG("%s: image:'%p' morp:'%s'\n", __func__, &image, morp);

    util_fite((morp == NULL), LOG_ERR("Morphology can not be NULL!\n"));

    if (!strcmp("dilation", morp)) {
	util_fit((morp_apply_dilation(image) != 0));
    } else if (!strcmp("erosion", morp)) {
	util_fit((morp_apply_erosion(image) != 0));
    } else if (!strcmp("open", morp)) {
	util_fit((morp_apply_open(image) != 0));
    } else if (!strcmp("close", morp)) {
	util_fit((morp_apply_close(image) != 0));
    } else {
	LOG_ERR("Morphology '%s' is not supported!\n", morp);
	goto fail;
    }

    goto success;

fail:
    ret = -1;

success:
    return ret;
}
