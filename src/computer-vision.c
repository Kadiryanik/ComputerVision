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

    LOG_DBG("%s: '%s'!\n", __func__, filename);

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

    LOG_INFO("File load succeed!\n");
    goto success;

fail:
    LOG_ERR("%s failed!\n\n", __func__);
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

    LOG_DBG("%s: in:'%s' out:'%s'\n", __func__, input_filename, output_filename);

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

    LOG_DBG("Trying to convert '%s' to gray scale!\n", input_filename);

    util_fit(((image = bmp_load(input_filename)) == NULL));
    util_fit(((intensity = bmp_convert_to_intensity(*image)) == NULL));
    util_fit(((gray_scale_image = bmp_convert_from_intensity(*intensity)) == NULL));
    util_fit(((bmp_save(output_filename ? output_filename : GRAY_SCALE_IMAGE_PATH,
			*gray_scale_image)) != 0));

    LOG_INFO("'%s' succesfully saved!\n",
	    output_filename ? output_filename : GRAY_SCALE_IMAGE_PATH);
    goto success;

fail:
    LOG_ERR("%s failed!\n\n", __func__);
    ret = -1;

success:
    sfree_image(image);
    sfree_image(intensity);
    sfree_image(gray_scale_image);
    return ret;
}

/*------------------------------------------------------------------------------*/
int cv_draw_tests(const char *input_filename, const char *output_filename,
	plus_t plus, rectangle_t rect, circle_t circle, ellipse_t ellipse)
{
    int ret = 0;
    image_t *image = NULL, *intensity = NULL, *draw_test_image = NULL;

    LOG_DBG("Trying to draw shapes!\n");

    util_fit(((image = bmp_load(input_filename)) == NULL));
    util_fit(((intensity = bmp_convert_to_intensity(*image)) == NULL));

    draw_plus(*intensity, plus);
    draw_rect(*intensity, rect);
    draw_circle(*intensity, circle);
    draw_ellipse(*intensity, ellipse);

    util_fit(((draw_test_image = bmp_convert_from_intensity(*intensity)) == NULL));
    util_fit(((bmp_save(output_filename ? output_filename : DRAW_TEST_IMAGE_PATH,
			*draw_test_image)) != 0));

    LOG_INFO("'%s' succesfully saved!\n",
	    output_filename ? output_filename : DRAW_TEST_IMAGE_PATH);
    goto success;

fail:
    LOG_ERR("%s failed!\n\n", __func__);
    ret = -1;

success:
    sfree_image(image);
    sfree_image(intensity);
    sfree_image(draw_test_image);
    return ret;
}

/*------------------------------------------------------------------------------*/
int cv_crop_image(const char *input_filename, const char *output_filename, rectangle_t rect)
{
    int ret = 0;
    image_t *image = NULL, *cropped_image = NULL;

    LOG_DBG("Trying to crop image! [%u,%u,%u,%u]\n",
	    rect.x, rect.y, rect.width, rect.height);

    util_fit(((image = bmp_load(input_filename)) == NULL));
    util_fit(((cropped_image = bmp_crop_image(*image, rect)) == NULL));
    util_fit(((bmp_save(output_filename ? output_filename : CROP_IMAGE_PATH,
			*cropped_image)) != 0));

    LOG_INFO("'%s' succesfully saved!\n",
	    output_filename ? output_filename : CROP_IMAGE_PATH);
    goto success;

fail:
    LOG_ERR("%s failed!\n\n", __func__);
    ret = -1;

success:
    sfree_image(image);
    sfree_image(cropped_image);
    return ret;
}

/*------------------------------------------------------------------------------*/
int cv_apply_mask(const char *input_filename, const char *output_filename,
	const char *mask_filename)
{
    int ret = 0;
    image_t *image = NULL, *intensity = NULL, *masked_image = NULL;
    mask_t *mask = NULL;

    LOG_DBG("Trying to apply mask!\n");

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
    LOG_ERR("%s failed!\n\n", __func__);
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

    LOG_DBG("%s: in:'%s' out:'%s' morp:'%s'\n", __func__, input_filename, output_filename, morp);

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

