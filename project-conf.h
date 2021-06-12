#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_

/*------------------------------------------------------------------------------*/
#define LOG_FEATURE_ENABLED	1 /* Disable for performance update */
#define LOG_LEVEL_CONF_CV	LOG_LEVEL_INFO

/*------------------------------------------------------------------------------*/
#define NBR_CONF_HFL		4 /* Check nbr_hfl in morphology.c for more detail */

/*------------------------------------------------------------------------------*/
#define FE_MATCH_EPSILON	0.001

/*------------------------------------------------------------------------------*/
#define SUPPORTED_FEATURES_NOE	7 /* In any changes, update _fe_get function too. */

/*------------------------------------------------------------------------------*/
#define BINARY_SCALE_IMAGE_PATH	    "images/binary.bmp"
#define GRAY_SCALE_IMAGE_PATH	    "images/grayscale.bmp"
#define DRAW_TEST_IMAGE_PATH	    "images/draw-test.bmp"
#define CROP_IMAGE_PATH		    "images/cropped.bmp"
#define MASK_IMAGE_PATH		    "images/mask.bmp"
#define MORP_TESTS_IMAGE_PATH	    "images/morphology.bmp"
#define REGIONS_IMAGE_PATH	    "images/regions.bmp"

#define FE_SINGLE_RESULT_PATH	    "db/single-result.txt"
#define FE_MULTI_RESULT_PATH	    "db/multi-result.txt"
#define FE_TEST_RESULT_IMAGE_PATH   "images/result.bmp"

#endif /* PROJECT_CONF_H_ */
