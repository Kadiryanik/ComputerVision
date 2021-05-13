#include <stdlib.h>
#include <string.h>

#include "computer-vision.h"
#include "log.h"
#include "util.h"
#include "k-means.h"

#ifndef LOG_LEVEL_CONF_CV
#define LOG_LEVEL LOG_LEVEL_ERR
#else /* LOG_LEVEL_CONF_CV */
#define LOG_LEVEL LOG_LEVEL_CONF_CV
#endif /* LOG_LEVEL_CONF_CV */

/*------------------------------------------------------------------------------*/
int convert_binary(const char *filename)
{
    int ret = 0, threshold = 0;
    uint32_t i = 0;
    image_t *image = NULL, *intensity = NULL, *binary_image = NULL,
	    *binary_image_bmp = NULL;

    LOG_DBG("Trying to convert '%s' to binary!\n", filename);

    util_fit(((image = bmp_load(filename)) == NULL));
    util_fit(((intensity = convert_bmp_to_intensity(*image)) == NULL));
    util_fit(((threshold = kmeans_get_thold(2, *intensity)) < 0));

    /* Allocate image and copy size fields into it.
     * Ignore the buf pointer we will set before use */
    util_fite(((binary_image = (image_t *)calloc(1, sizeof(image_t))) == NULL),
	    LOG_ERR("BinaryImage allocation failed!\n"));
    memcpy(binary_image, intensity, sizeof(image_t));

    util_fite(((binary_image->buf = (uint8_t *)malloc(binary_image->size *
	    sizeof(uint8_t))) == NULL), LOG_ERR("BinaryImage data allocation failed!\n"));
    for (i = 0; i < binary_image->size; i++) {
	binary_image->buf[i] = (intensity->buf[i] > threshold) ? 255 : 0;
    }

    util_fit(((binary_image_bmp = convert_intensity_to_bmp(*binary_image)) == NULL));
    util_fit((bmp_save(BINARY_SCALE_IMAGE_PATH, *binary_image_bmp) != 0));

    LOG_INFO("'%s' succesfully saved!\n", BINARY_SCALE_IMAGE_PATH);
    goto success;

fail:
    LOG_ERR("%s failed!\n\n", __func__);
    ret = -1;

success:
    sfree_image(image);
    sfree_image(intensity);
    sfree_image(binary_image);
    sfree_image(binary_image_bmp);
    return ret;
}

/*------------------------------------------------------------------------------*/
int convert_grayscale(const char *filename)
{
    int ret = 0;
    image_t *image = NULL, *intensity = NULL, *gray_scale_image = NULL;

    LOG_DBG("Trying to convert '%s' to gray scale!\n", filename);

    util_fit(((image = bmp_load(filename)) == NULL));
    util_fit(((intensity = convert_bmp_to_intensity(*image)) == NULL));
    util_fit(((gray_scale_image = convert_intensity_to_bmp(*intensity)) == NULL));
    util_fit(((bmp_save(GRAY_SCALE_IMAGE_PATH, *gray_scale_image)) != 0));

    LOG_INFO("'%s' succesfully saved!\n", GRAY_SCALE_IMAGE_PATH);
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
int draw_tests(const char *filename, plus_t plus, rectangle_t rect,
	circle_t circle, ellipse_t ellipse)
{
    int ret = 0;
    image_t *image = NULL, *intensity = NULL, *draw_test_image = NULL;

    LOG_DBG("Trying to draw shapes!\n");

    util_fit(((image = bmp_load(filename)) == NULL));
    util_fit(((intensity = convert_bmp_to_intensity(*image)) == NULL));

    draw_plus(*intensity, plus);
    draw_rect(*intensity, rect);
    draw_circle(*intensity, circle);
    draw_ellipse(*intensity, ellipse);

    util_fit(((draw_test_image = convert_intensity_to_bmp(*intensity)) == NULL));
    util_fit(((bmp_save(DRAW_TEST_IMAGE_PATH, *draw_test_image)) != 0));

    LOG_INFO("'%s' succesfully saved!\n", DRAW_TEST_IMAGE_PATH);
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
int test_bmp_file(const char *filename)
{
    int ret = 0;
    image_t *image = NULL;

    LOG_PRINT_("Trying to load '%s'\n", filename);

    util_fit(((image = bmp_load(filename)) == NULL));

    LOG_PRINT_("File load succeed!\n");
    goto success;

fail:
    LOG_ERR("%s failed!\n\n", __func__);
    ret = -1;

success:
    sfree_image(image);
    return ret;
}

