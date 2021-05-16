#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_

#define LOG_FEATURE_ENABLED	1 /* Disable for performance update */

#define LOG_LEVEL_CONF_BMP	LOG_LEVEL_ERR
#define LOG_LEVEL_CONF_UTIL	LOG_LEVEL_ERR
#define LOG_LEVEL_CONF_KMEANS	LOG_LEVEL_ERR
#define LOG_LEVEL_CONF_MASK	LOG_LEVEL_INFO
#define LOG_LEVEL_CONF_MORP	LOG_LEVEL_INFO
#define LOG_LEVEL_CONF_CV	LOG_LEVEL_INFO
#define LOG_LEVEL_CONF_TEST	LOG_LEVEL_INFO

/*------------------------------------------------------------------------------*/
#define BINARY_SCALE_IMAGE_PATH	    "images/binary.bmp"
#define GRAY_SCALE_IMAGE_PATH	    "images/grayscale.bmp"
#define DRAW_TEST_IMAGE_PATH	    "images/draw-test.bmp"
#define CROP_IMAGE_PATH		    "images/cropped.bmp"
#define MASK_IMAGE_PATH		    "images/mask.bmp"
#define MORP_TESTS_IMAGE_PATH	    "images/morphology.bmp"

#endif /* PROJECT_CONF_H_ */
