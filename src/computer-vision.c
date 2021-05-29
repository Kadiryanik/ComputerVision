/**
 * \file
 *	ComputerVision/ImageProcessing functions
 *
 * \author
 *	Kadir Yanık <kdrynkk@gmail.com>
 */

#include <stdlib.h>
#include <string.h>

#include "computer-vision.h"
#include "log.h"
#include "util.h"
#include "k-means.h"
#include "mask.h"
#include "morphology.h"

#ifndef LOG_LEVEL_CONF_CV
#define LOG_LEVEL LOG_LEVEL_ERR
#else /* LOG_LEVEL_CONF_CV */
#define LOG_LEVEL LOG_LEVEL_CONF_CV
#endif /* LOG_LEVEL_CONF_CV */

/*------------------------------------------------------------------------------*/
static image_t* _cv_get_binary_image(const char *filename)
{
    int threshold = 0;
    uint32_t i = 0;
    image_t *image = NULL, *intensity = NULL, *binary_image = NULL;

    LOG_DBG("filename:'%s'\n", filename);

    util_fit(((image = bmp_load(filename)) == NULL));
    util_fit(((intensity = bmp_convert_to_intensity(*image)) == NULL));
    util_fit(((threshold = kmeans_get_thold(2, *intensity)) < 0));

    /* Allocate image and copy size fields into it.
     * Ignore the buf pointer we will set it before use */
    util_fite(((binary_image = (image_t *)calloc(1, sizeof(image_t))) == NULL),
	    LOG_ERR("BinaryImage allocation failed!\n"));
    memcpy(binary_image, intensity, sizeof(image_t));

    util_fite(((binary_image->buf = (uint8_t *)malloc(binary_image->size *
	    sizeof(uint8_t))) == NULL), LOG_ERR("BinaryImage data allocation failed!\n"));
    for (i = 0; i < binary_image->size; i++) {
	binary_image->buf[i] = (intensity->buf[i] > threshold) ? COLOUR_WHITE : COLOUR_BLACK;
    }

    goto success;

fail:
    LOG_ERR("%s failed!\n", __func__);
    sfree_image(binary_image);

success:
    sfree_image(image);
    sfree_image(intensity);
    return binary_image;
}

/*------------------------------------------------------------------------------*/
int cv_test_bmp_file(const char *filename)
{
    int ret = 0;
    image_t *image = NULL;

    LOG_INFO("Trying to load '%s'\n", filename);

    util_fit(((image = bmp_load(filename)) == NULL));

    LOG_INFO("'%s' loaded successfully!\n", filename);
    goto success;

fail:
    LOG_ERR("%s failed!\n", __func__);
    ret = -1;

success:
    sfree_image(image);
    return ret;
}

/*------------------------------------------------------------------------------*/
int cv_convert_binary(const char *input_filename, const char *output_filename)
{
    int ret = 0;
    image_t *binary_image = NULL, *binary_image_bmp = NULL;

    LOG_DBG("input_filename:'%s' output_image:'%s'\n", input_filename, output_filename);

    util_fit(((binary_image = _cv_get_binary_image(input_filename)) == NULL));
    util_fit(((binary_image_bmp = bmp_convert_from_intensity(*binary_image)) == NULL));
    util_fit((bmp_save(output_filename ? output_filename : BINARY_SCALE_IMAGE_PATH,
		    *binary_image_bmp) != 0));

    LOG_INFO("'%s' succesfully saved!\n",
	    output_filename ? output_filename : BINARY_SCALE_IMAGE_PATH);
    goto success;

fail:
    LOG_ERR("%s failed!\n", __func__);
    ret = -1;

success:
    sfree_image(binary_image);
    sfree_image(binary_image_bmp);
    return ret;
}

/*------------------------------------------------------------------------------*/
int cv_convert_grayscale(const char *input_filename, const char *output_filename)
{
    int ret = 0;
    image_t *image = NULL, *intensity = NULL, *gray_scale_image = NULL;

    LOG_DBG("input_filename:'%s' output_image:'%s'\n", input_filename, output_filename);

    util_fit(((image = bmp_load(input_filename)) == NULL));
    util_fit(((intensity = bmp_convert_to_intensity(*image)) == NULL));

    util_fit(((gray_scale_image = bmp_convert_from_intensity(*intensity)) == NULL));

    util_fit(((bmp_save(output_filename ? output_filename : GRAY_SCALE_IMAGE_PATH,
			*gray_scale_image)) != 0));

    LOG_INFO("'%s' succesfully saved!\n",
	    output_filename ? output_filename : GRAY_SCALE_IMAGE_PATH);
    goto success;

fail:
    LOG_ERR("%s failed!\n", __func__);
    ret = -1;

success:
    sfree_image(image);
    sfree_image(intensity);
    sfree_image(gray_scale_image);
    return ret;
}

/*------------------------------------------------------------------------------*/
int cv_draw(const char *input_filename, const char *output_filename,
	const char *draw_filename)
{
    int ret = 0;
    image_t *image = NULL, *rgb_image = NULL, *drawed_image = NULL;

    LOG_DBG("input_filename:'%s' output_image:'%s' draw_filename:'%s'\n",
	    input_filename, output_filename, draw_filename);

    util_fit(((image = bmp_load(input_filename)) == NULL));
    util_fit(((rgb_image = bmp_convert_to_rgb(*image)) == NULL));

    util_fit((draw_multi_shapes(*rgb_image, draw_filename) != 0));

    util_fit(((drawed_image = bmp_convert_from_rgb(*rgb_image)) == NULL));
    util_fit(((bmp_save(output_filename ? output_filename : DRAW_TEST_IMAGE_PATH,
			*drawed_image)) != 0));

    LOG_INFO("'%s' succesfully saved!\n",
	    output_filename ? output_filename : DRAW_TEST_IMAGE_PATH);
    goto success;

fail:
    LOG_ERR("%s failed!\n", __func__);
    ret = -1;

success:
    sfree_image(image);
    sfree_image(rgb_image);
    sfree_image(drawed_image);
    return ret;
}

/*------------------------------------------------------------------------------*/
int cv_crop_image(const char *input_filename, const char *output_filename, rectangle_t rect)
{
    int ret = 0;
    image_t *image = NULL, *rgb_image = NULL, *cropped_image = NULL,
	    *cropped_image_bmp = NULL;

    LOG_DBG("input_filename:'%s' output_filename:'%s' rect:[%u,%u,%u,%u]\n",
	    input_filename, output_filename, rect.x, rect.y, rect.width, rect.height);

    util_fit(((image = bmp_load(input_filename)) == NULL));
    util_fit(((rgb_image = bmp_convert_to_rgb(*image)) == NULL));

    util_fit(((cropped_image = bmp_crop_image(*rgb_image, rect)) == NULL));

    util_fit(((cropped_image_bmp = bmp_convert_from_rgb(*cropped_image)) == NULL));
    util_fit(((bmp_save(output_filename ? output_filename : CROP_IMAGE_PATH,
			*cropped_image_bmp)) != 0));

    LOG_INFO("'%s' succesfully saved!\n",
	    output_filename ? output_filename : CROP_IMAGE_PATH);
    goto success;

fail:
    LOG_ERR("%s failed!\n", __func__);
    ret = -1;

success:
    sfree_image(image);
    sfree_image(rgb_image);
    sfree_image(cropped_image);
    sfree_image(cropped_image_bmp);
    return ret;
}

/*------------------------------------------------------------------------------*/
int cv_apply_mask(const char *input_filename, const char *output_filename,
	const char *mask_filename)
{
    int ret = 0;
    image_t *image = NULL, *intensity = NULL, *masked_image = NULL;
    mask_t *mask = NULL;

    LOG_DBG("input_filename:'%s' output_filename:'%s' mask_filename:'%s'\n",
	    input_filename, output_filename, mask_filename);

    /* Try to get mask first */
    util_fit(((mask = mask_read_from_file(mask_filename)) == NULL));
    util_fit(((image = bmp_load(input_filename)) == NULL));
    util_fit(((intensity = bmp_convert_to_intensity(*image)) == NULL));

    util_fit(((mask_apply(*intensity, *mask)) != 0));

    util_fit(((masked_image = bmp_convert_from_intensity(*intensity)) == NULL));
    util_fit(((bmp_save(output_filename ? output_filename : MASK_IMAGE_PATH,
			*masked_image)) != 0));

    LOG_INFO("'%s' succesfully saved!\n",
	    output_filename ? output_filename : MASK_IMAGE_PATH);
    goto success;

fail:
    LOG_ERR("%s failed!\n", __func__);
    ret = -1;

success:
    sfree_mask(mask);
    sfree_image(image);
    sfree_image(intensity);
    sfree_image(masked_image);
    return ret;
}

/*------------------------------------------------------------------------------*/
int cv_apply_morphology(const char *input_filename, const char *output_filename,
	const char *morp)
{
    int ret = 0;
    image_t *binary_image = NULL, *binary_image_bmp = NULL;

    LOG_DBG("input_filename:'%s' output_filename:'%s' morp:'%s'\n",
	    input_filename, output_filename, morp);

    util_fit(((binary_image = _cv_get_binary_image(input_filename)) == NULL));

    util_fit((morp_apply(*binary_image, morp) != 0));

    util_fit(((binary_image_bmp = bmp_convert_from_intensity(*binary_image)) == NULL));
    util_fit((bmp_save(output_filename ? output_filename : MORP_TESTS_IMAGE_PATH,
		    *binary_image_bmp) != 0));

    LOG_INFO("'%s' succesfully saved!\n",
	    output_filename ? output_filename : MORP_TESTS_IMAGE_PATH);
    goto success;

fail:
    LOG_ERR("%s failed!\n", __func__);
    ret = -1;

success:
    sfree_image(binary_image);
    sfree_image(binary_image_bmp);
    return ret;
}

/*------------------------------------------------------------------------------*/
int cv_identify_regions(const char *input_filename, const char *output_filename)
{
    int ret = 0;
    image_t *binary_image = NULL, *binary_image_bmp = NULL, *regions_image = NULL;
    regions_t regions = { .noe = 0, .region = NULL };

    LOG_DBG("input_filename:'%s' output_filename:'%s'\n",
	    input_filename, output_filename);

    util_fit(((binary_image = _cv_get_binary_image(input_filename)) == NULL));

    /* First apply open to eliminate noise */
    util_fit((morp_apply(*binary_image, "open") != 0));
    util_fit(((regions_image = morp_identify_regions(*binary_image, &regions)) == NULL));

    util_fit(((binary_image_bmp = bmp_convert_from_intensity(*regions_image)) == NULL));

    util_fit((bmp_save(output_filename ? output_filename : REGIONS_IMAGE_PATH,
		    *binary_image_bmp) != 0));

    LOG_INFO("'%s' succesfully saved!\n",
	    output_filename ? output_filename : REGIONS_IMAGE_PATH);
    goto success;

fail:
    LOG_ERR("%s failed!\n", __func__);
    ret = -1;

success:
    sfree_image(binary_image);
    sfree_image(binary_image_bmp);
    sfree_image(regions_image);
    sfree(regions.region);
    return ret;
}

