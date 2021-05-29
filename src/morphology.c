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
/* HFL	    : Half Frame Len
 * Example  : HFL=5 -> Checks 10x10 frame for neighbors
 * Note	    : If this is too small then X. pass count will increase */
#define NBR_HFL 5

enum region_states {
    REGION_DATA = 0,
    REGION_BACKGROUND = 1
};

/*------------------------------------------------------------------------------*/
static int _morp_apply(image_t image, mask_t mask, uint8_t check_value)
{
    int ret = 0;
    uint8_t *temp_buf = NULL;
    uint32_t i = 0, j = 0, k = 0, l = 0, mask_center_i = 0, mask_center_j = 0;

    LOG_DBG("image:%p mask:%p, check_val:%u\n", &image, &mask, check_value);

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

    LOG_DBG("image:%p morp:'%s'\n", &image, morp);

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

/*------------------------------------------------------------------------------*/
/* TODO: make label calculation on int16_t buffer for bigger contents.
 * Reset the label ids after fixing overlapping for balanced colors */
image_t* morp_identify_regions(image_t image, regions_t *regions)
{
    uint32_t i = 0, j = 0, t = 0, orig_pos = 0, nbr_pos = 0;
    int8_t k = 0, l = 0;
    uint8_t total_label = REGION_BACKGROUND, nbr_label = 0, nbr_min_label = 0,
	    nbr_max_label = 0, region_offset = 0, found = 0;

    image_t *new_image = NULL;

    LOG_DBG("image:%p\n", &image);

    util_fite(((new_image = (image_t *)calloc(1, sizeof(image_t))) == NULL),
	    LOG_ERR("Image allocation failed\n"));
    new_image->size = image.width * image.height;

    util_fite(((new_image->buf = (uint8_t *)malloc((new_image->size) *
			sizeof(uint8_t))) == NULL), LOG_ERR("Image data allocation failed\n"));
    new_image->width = image.width;
    new_image->height = image.height;
    new_image->cb = 1;

    for (i = 0; i < image.width * image.height; i++) {
	new_image->buf[i] = image.buf[i] ? REGION_BACKGROUND : REGION_DATA;
    }

    /* First pass: Set the labels with checking neighbors.
     * Pass the border pixels for eliminate extra checking */
    for (i = NBR_HFL; i < image.height - NBR_HFL; i++) {
        for (j = NBR_HFL; j < image.width - NBR_HFL; j++) {
	    orig_pos = i * image.width + j;
	    if (new_image->buf[orig_pos] != REGION_DATA) continue;

	    nbr_label = 0;
	    for (k = -NBR_HFL; k <= NBR_HFL; k++) {
		for (l = -NBR_HFL; l <= NBR_HFL; l++) {
		    nbr_pos = (i + k) * image.width + j + l;
		    if (new_image->buf[nbr_pos] > REGION_BACKGROUND) {
			nbr_label = new_image->buf[nbr_pos];
			break;
		    }
		}
	    }
	    if (nbr_label == 0) {
		nbr_label = ++total_label;
	    }
	    new_image->buf[orig_pos] = nbr_label;
	}
    }

    /* Second pass: Check it out if there are different labels in the neighbor frame.
     * Pass the border pixels for eliminate extra checking */
    for (i = NBR_HFL; i < image.height - NBR_HFL; i++) {
        for (j = NBR_HFL; j < image.width - NBR_HFL; j++) {
	    orig_pos = i * image.width + j;
	    if (new_image->buf[orig_pos] == REGION_BACKGROUND) continue;

	    nbr_min_label = new_image->buf[orig_pos];
	    nbr_max_label = new_image->buf[orig_pos];
	    for (k = -NBR_HFL; k <= NBR_HFL; k++) {
		for (l = -NBR_HFL; l <= NBR_HFL; l++) {
		    nbr_pos = (i + k) * image.width + j + l;
		    if (new_image->buf[nbr_pos] == REGION_BACKGROUND) continue;

		    nbr_label = new_image->buf[nbr_pos];
		    if (new_image->buf[nbr_pos] > nbr_max_label) {
			nbr_max_label = new_image->buf[nbr_pos];
		    }
		    if(new_image->buf[nbr_pos] < nbr_min_label) {
			nbr_min_label = new_image->buf[nbr_pos];
		    }
		}
	    }
	    if (nbr_min_label != nbr_max_label) {
		LOG_DBG("Overlapping labels [%u & %u] on image[%u][%u]\n",
			nbr_min_label, nbr_max_label, i, j);
		/* X. pass: Fix overlapping labels in the whole picture. */
		for (t = 0; t < image.height * image.width; t++) {
		    if (new_image->buf[t] == nbr_max_label) {
			new_image->buf[t] = nbr_min_label;
		    }
		}

		/* for calculating real total label after fixing overlapping labels */
		total_label--;
	    }
	}
    }
    total_label = total_label - REGION_BACKGROUND;

    regions->noe = total_label;
    util_fite(((regions->region = (region_t *)calloc(regions->noe, sizeof(region_t))) == NULL),
	    LOG_ERR("Regions->region allocation failed\n"));

    for (i = 0; i < image.height; i++) {
	for (j = 0; j < image.width; j++) {
	    orig_pos = i * image.width + j;
	    if (new_image->buf[orig_pos] == REGION_BACKGROUND) continue;

	    /* Check current regions labels, if not exist set new */
	    found = 0;
	    for (k = 0; k < region_offset; k++) {
		if (new_image->buf[orig_pos] == regions->region[k].old_label) {
		    found = 1;
		    /* update region */
		    if (i < regions->region[k].rect.x) regions->region[k].rect.x = i;
		    if (j < regions->region[k].rect.y) regions->region[k].rect.y = j;
		    if (i > regions->region[k].rect.height) regions->region[k].rect.height = i;
		    if (j > regions->region[k].rect.width) regions->region[k].rect.width = j;
		    break;
		}
	    }
	    if (found) continue;

	    util_fite((region_offset >= regions->noe),
		    LOG_ERR("FATAL: label count and image mismatch! (%u)\n", region_offset));

	    /* initialize region */
	    regions->region[region_offset].old_label = new_image->buf[orig_pos];
	    regions->region[region_offset].label = region_offset;
	    regions->region[region_offset].rect.x = i;
	    regions->region[region_offset].rect.y = j;
	    regions->region[region_offset].rect.height = 0;
	    regions->region[region_offset].rect.width = 0;

	    LOG_DBG("region[%d]=%u->%u\n", region_offset, regions->region[region_offset].old_label,
		    regions->region[region_offset].label);
	    region_offset++;
	}
    }
    util_fite((region_offset != regions->noe),
	    LOG_ERR("FATAL: region_offset and noe mismatch!\n"));

    /* Final pass: set the different grayscale color for each regions */
    LOG_DBG("Total label = %u\n", total_label);
    for (i = 0; i < image.width * image.height; i++) {
	if (new_image->buf[i] == REGION_BACKGROUND) {
	    new_image->buf[i] = COLOUR_WHITE;
	} else {
	    /* Use new label for coloring */
	    for (j = 0; j < regions->noe; j++) {
		if (new_image->buf[i] == regions->region[j].old_label) {
		    new_image->buf[i] = regions->region[j].label * (240 / (total_label));
		    break;
		}
	    }
	}
    }

    /* width and height holds pixels which is end of the rectangle, set correct values */
    for (i = 0; i < regions->noe; i++) {
	regions->region[i].rect.height -= regions->region[i].rect.x;
	regions->region[i].rect.width -= regions->region[i].rect.y;
    }
    goto success;

fail:
    sfree_image(new_image);

success:
    return new_image;
}

