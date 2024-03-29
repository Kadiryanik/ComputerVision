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
#include "feature-extraction.h"

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
	binary_image->buf[i] = (intensity->buf[i] > threshold) ? COLOR_BG : COLOR_FG;
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
static image_t* _cv_get_regions(const char *input_filename, regions_t *regions)
{
    image_t *binary_image = NULL, *regions_image = NULL;

    LOG_DBG("input_filename:'%s' regions:%p\n",
	    input_filename, regions);

    util_fit(((binary_image = _cv_get_binary_image(input_filename)) == NULL));

    /* First apply open to eliminate noise */
    util_fit((morp_apply(*binary_image, "open") != 0));
    util_fit(((regions_image = morp_identify_regions(*binary_image, regions)) == NULL));

    goto success;

fail:
    LOG_ERR("%s failed!\n", __func__);
    sfree_image(regions_image);

success:
    sfree_image(binary_image);
    return regions_image;
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

    output_filename = (output_filename != NULL) ? output_filename : BINARY_SCALE_IMAGE_PATH;

    LOG_DBG("input_filename:'%s' output_image:'%s'\n", input_filename, output_filename);

    util_fit(((binary_image = _cv_get_binary_image(input_filename)) == NULL));
    util_fit(((binary_image_bmp = bmp_convert_from_intensity(*binary_image)) == NULL));
    util_fit((bmp_save(output_filename, *binary_image_bmp) != 0));

    LOG_INFO("'%s' succesfully saved!\n", output_filename);
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

    output_filename = (output_filename != NULL) ? output_filename : GRAY_SCALE_IMAGE_PATH;

    LOG_DBG("input_filename:'%s' output_image:'%s'\n", input_filename, output_filename);

    util_fit(((image = bmp_load(input_filename)) == NULL));
    util_fit(((intensity = bmp_convert_to_intensity(*image)) == NULL));

    util_fit(((gray_scale_image = bmp_convert_from_intensity(*intensity)) == NULL));

    util_fit(((bmp_save(output_filename, *gray_scale_image)) != 0));

    LOG_INFO("'%s' succesfully saved!\n", output_filename);
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

    output_filename = (output_filename != NULL) ? output_filename : DRAW_TEST_IMAGE_PATH;

    LOG_DBG("input_filename:'%s' output_image:'%s' draw_filename:'%s'\n",
	    input_filename, output_filename, draw_filename);

    util_fit(((image = bmp_load(input_filename)) == NULL));
    util_fit(((rgb_image = bmp_convert_to_rgb(*image)) == NULL));

    util_fit((draw_multi_shapes(*rgb_image, draw_filename, 0) != 0));

    util_fit(((drawed_image = bmp_convert_from_rgb(*rgb_image)) == NULL));
    util_fit(((bmp_save(output_filename, *drawed_image)) != 0));

    LOG_INFO("'%s' succesfully saved!\n", output_filename);
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

    output_filename = (output_filename != NULL) ? output_filename : CROP_IMAGE_PATH;

    LOG_DBG("input_filename:'%s' output_filename:'%s' rect:[%u,%u,%u,%u]\n",
	    input_filename, output_filename, rect.x, rect.y, rect.width, rect.height);

    util_fit(((image = bmp_load(input_filename)) == NULL));
    util_fit(((rgb_image = bmp_convert_to_rgb(*image)) == NULL));

    util_fit(((cropped_image = bmp_crop_image(*rgb_image, rect)) == NULL));

    util_fit(((cropped_image_bmp = bmp_convert_from_rgb(*cropped_image)) == NULL));
    util_fit(((bmp_save(output_filename, *cropped_image_bmp)) != 0));

    LOG_INFO("'%s' succesfully saved!\n", output_filename);
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

    output_filename = (output_filename != NULL) ? output_filename : MASK_IMAGE_PATH;

    LOG_DBG("input_filename:'%s' output_filename:'%s' mask_filename:'%s'\n",
	    input_filename, output_filename, mask_filename);

    /* Try to get mask first */
    util_fit(((mask = mask_read_from_file(mask_filename)) == NULL));
    util_fit(((image = bmp_load(input_filename)) == NULL));
    util_fit(((intensity = bmp_convert_to_intensity(*image)) == NULL));

    util_fit(((mask_apply(*intensity, *mask)) != 0));

    util_fit(((masked_image = bmp_convert_from_intensity(*intensity)) == NULL));
    util_fit(((bmp_save(output_filename, *masked_image)) != 0));

    LOG_INFO("'%s' succesfully saved!\n", output_filename);
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

    output_filename = (output_filename != NULL) ? output_filename : MORP_TESTS_IMAGE_PATH;

    LOG_DBG("input_filename:'%s' output_filename:'%s' morp:'%s'\n",
	    input_filename, output_filename, morp);

    util_fit(((binary_image = _cv_get_binary_image(input_filename)) == NULL));

    util_fit((morp_apply(*binary_image, morp) != 0));

    util_fit(((binary_image_bmp = bmp_convert_from_intensity(*binary_image)) == NULL));
    util_fit((bmp_save(output_filename, *binary_image_bmp) != 0));

    LOG_INFO("'%s' succesfully saved!\n", output_filename);
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
    image_t *regions_image_bmp = NULL, *regions_image = NULL;
    regions_t regions = { .noe = 0, .region = NULL };

    output_filename = (output_filename != NULL) ? output_filename : REGIONS_IMAGE_PATH;

    LOG_DBG("input_filename:'%s' output_filename:'%s'\n",
	    input_filename, output_filename);

    util_fit(((regions_image = _cv_get_regions(input_filename, &regions)) == NULL));
    /* scale colors */
    morp_colorize_regions(*regions_image, regions.noe);

    util_fit(((regions_image_bmp = bmp_convert_from_intensity(*regions_image)) == NULL));

    util_fit((bmp_save(output_filename, *regions_image_bmp) != 0));

    LOG_INFO("'%s' succesfully saved!\n", output_filename);
    goto success;

fail:
    LOG_ERR("%s failed!\n", __func__);
    ret = -1;

success:
    sfree_image(regions_image_bmp);
    sfree_image(regions_image);
    sfree(regions.region);
    return ret;
}

/*------------------------------------------------------------------------------*/
int cv_feature_extraction_single(const char *input_filename, const char *output_filename)
{
    int ret = 0;
    image_t *regions_image = NULL;
    regions_t regions = { .noe = 0, .region = NULL };
    features_t *features_avg = NULL;

    output_filename = (output_filename != NULL) ? output_filename : FE_SINGLE_RESULT_PATH;

    LOG_DBG("input_filename:'%s' output_filename:'%s'\n",
	    input_filename, output_filename);

    util_fit(((regions_image = _cv_get_regions(input_filename, &regions)) == NULL));
    util_fit(((features_avg = fe_get_avg(*regions_image, regions)) == NULL));
    util_fit((fe_save(output_filename, *features_avg) != 0));

    LOG_INFO("'%s' succesfully saved!\n", output_filename);
    goto success;

fail:
    LOG_ERR("%s failed!\n", __func__);
    ret = -1;

success:
    sfree_image(regions_image);
    sfree(regions.region);
    sfree_features(features_avg);
    return ret;
}

/*------------------------------------------------------------------------------*/
int cv_feature_extraction_multi(const char *input_filename, const char *output_filename)
{
    int ret = 0;
    class_t *classes = NULL, *current_class = NULL;
    str_node_t *current_filename = NULL;
    image_t *regions_image = NULL;
    regions_t regions = { .noe = 0, .region = NULL };

    output_filename = (output_filename != NULL) ? output_filename : FE_MULTI_RESULT_PATH;

    LOG_DBG("input_filename:'%s' output_filename:'%s'\n",
	    input_filename, output_filename);

    /* Get class image info from formatted input file */
    util_fit(((classes = fe_load_classes(input_filename)) == NULL));

    /* Calculate each class */
    current_class = classes;
    while (current_class != NULL) {
	current_filename = current_class->files;
	while (current_filename != NULL) {
	    util_fit(((regions_image = _cv_get_regions(current_filename->str, &regions)) == NULL));

	    /* update class features with regions */
	    util_fit((fe_classes_update(current_class, *regions_image, regions) != 0));
	    LOG_ERR("Class '%s' updated %p\n", current_class->name, current_class->features->feature);

	    sfree_image(regions_image);
	    sfree(regions.region);
	    current_filename = current_filename->next;
	}
	current_class = current_class->next;
    }
    /* Save calsses db to file */
    util_fit((fe_save_classes(output_filename, classes) != 0));

    LOG_INFO("'%s' succesfully saved!\n", output_filename);
    goto success;

fail:
    LOG_ERR("%s failed!\n", __func__);
    ret = -1;
    sfree_image(regions_image);
    sfree(regions.region);

success:
    fe_classes_free(&classes);

    return ret;
}

/*------------------------------------------------------------------------------*/
int cv_feature_extraction_test(const char *input_filename,
	const char *test_image_filename, const char *output_filename)
{
    int ret = 0;
    class_t *classes = NULL;
    image_t *regions_image = NULL, *drawed_image = NULL,
	    *rgb_image = NULL, *final_image = NULL;
    regions_t regions = { .noe = 0, .region = NULL };

    output_filename = (output_filename != NULL) ? output_filename : FE_TEST_RESULT_IMAGE_PATH;

    LOG_DBG("input_filename:'%s' output_filename:'%s'\n",
	    input_filename, output_filename);

    /* Get class features info from formatted input file */
    util_fit(((classes = fe_load_classes_with_features(input_filename)) == NULL));

    /* Get regions */
    util_fit(((regions_image = _cv_get_regions(test_image_filename, &regions)) == NULL));

    /* Load bmp data in rgb form */
    util_fit(((final_image = bmp_load(test_image_filename)) == NULL));
    util_fit(((rgb_image = bmp_convert_to_rgb(*final_image)) == NULL));

    /* Find nearest and mark region with class color on orig image */
    util_fit((fe_test(*regions_image, regions, *classes, *rgb_image) != 0));

    /* Save result image */
    util_fit(((drawed_image = bmp_convert_from_rgb(*rgb_image)) == NULL));
    util_fit((bmp_save(output_filename, *drawed_image) != 0));

    LOG_INFO("'%s' succesfully saved!\n", output_filename);
    goto success;

fail:
    LOG_ERR("%s failed!\n", __func__);
    ret = -1;

success:
    fe_classes_free(&classes);
    sfree(regions.region);
    sfree_image(regions_image);
    sfree_image(drawed_image);
    sfree_image(rgb_image);
    sfree_image(final_image);

    return ret;
}

/*------------------------------------------------------------------------------*/
int cv_feature_extraction(const char *type, const char *input_filename,
	const char *test_image_filename, const char *output_filename)
{
    int ret = 0;

    if (strcmp(type, "avg") == 0) {
	util_fit((cv_feature_extraction_single(input_filename, output_filename) != 0));
    } else if (strcmp(type, "learn") == 0) {
	util_fit((cv_feature_extraction_multi(input_filename, output_filename) != 0));
    } else if (strcmp(type, "test") == 0) {
	util_fite((test_image_filename == NULL),
		LOG_ERR("Feature extraction with 'test' needs test image file as input!\n"));
	util_fit((cv_feature_extraction_test(input_filename, test_image_filename,
			output_filename) != 0));
    } else {
	LOG_ERR("'%s' is not supperted for feature extraction!\n", type);
	goto fail;
    }

    goto success;

fail:
    ret = -1;

success:
    return ret;
}
