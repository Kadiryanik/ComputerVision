#include <stdlib.h>

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
    int ret = 0;
    int i = 0, threshold = 0, width = 0, height = 0, size = 0;
    uint8_t *buffer = NULL, *intensity = NULL, *binary_image = NULL, *binary_image_bmp = NULL;

    LOG_DBG("Trying to convert '%s' to binary!\n", filename);

    util_fit(((buffer = bmp_load(filename, &width, &height, &size)) == NULL));
    util_fit(((intensity = convert_bmp_to_intensity(buffer, width, height)) == NULL));
    util_fit(((threshold = kmeans_get_thold(2, intensity, width, height)) < 0));

    util_fite(((binary_image = (uint8_t *)malloc((width * height) * sizeof(uint8_t))) == NULL),
	    LOG_ERR("Binary-image buffer allocation failed!\n"));
    for (i = 0; i < width * height; i++) {
	binary_image[i] = (intensity[i] > threshold) ? 255 : 0;
    }

    util_fit(((binary_image_bmp = convert_intensity_to_bmp(binary_image, width, height, &size)) == NULL));
    util_fit((bmp_save(BINARY_SCALE_IMAGE_PATH, width, height, binary_image_bmp) != 0));

    LOG_INFO("'%s' succesfully saved!\n", BINARY_SCALE_IMAGE_PATH);
    goto success;

fail:
    LOG_ERR("%s failed!\n\n", __func__);
    ret = -1;

success:
    sfree(buffer);
    sfree(intensity);
    sfree(binary_image);
    sfree(binary_image_bmp);
    return ret;
}

/*------------------------------------------------------------------------------*/
int convert_grayscale(const char *filename)
{
    int ret = 0, width = 0, height = 0, size = 0;
    uint8_t *buffer = NULL, *intensity = NULL, *gray_scale_bmp_data = NULL;

    LOG_DBG("Trying to convert '%s' to gray scale!\n", filename);

    util_fit(((buffer = bmp_load(filename, &width, &height, &size)) == NULL));
    util_fit(((intensity = convert_bmp_to_intensity(buffer, width, height)) == NULL));
    util_fit(((gray_scale_bmp_data = convert_intensity_to_bmp(intensity, width, height, &size)) == NULL));
    util_fit(((bmp_save(GRAY_SCALE_IMAGE_PATH, width, height, gray_scale_bmp_data)) != 0));

    LOG_INFO("'%s' succesfully saved!\n", GRAY_SCALE_IMAGE_PATH);
    goto success;

fail:
    LOG_ERR("%s failed!\n\n", __func__);
    ret = -1;

success:
    sfree(buffer);
    sfree(intensity);
    sfree(gray_scale_bmp_data);
    return ret;
}

/*------------------------------------------------------------------------------*/
int test_bmp_file(const char *filename)
{
    int ret = 0, width = 0, height = 0, size = 0;
    uint8_t *buffer = NULL;

    LOG_PRINT_("Trying to load '%s'\n", filename);

    util_fit(((buffer = bmp_load(filename, &width, &height, &size)) == NULL));

    LOG_PRINT_("File load succeed!\n");
    goto success;

fail:
    LOG_ERR("%s failed!\n\n", __func__);
    ret = -1;

success:
    sfree(buffer);
    return ret;
}

