/**
 * \file
 *	ComputerVision/ImageProcessing functions
 *
 * \author
 *	Kadir Yanık <kdrynkk@gmail.com>
 */

#ifndef COMPUTER_VISION_H_
#define COMPUTER_VISION_H_

#include "bmp.h"

/*------------------------------------------------------------------------------*/
int cv_test_bmp_file(const char *);
int cv_convert_binary(const char *, const char *);
int cv_convert_grayscale(const char *, const char *);
int cv_draw(const char *, const char *, const char *);
int cv_crop_image(const char *, const char *, rectangle_t);
int cv_apply_mask(const char *, const char *, const char *);
int cv_apply_morphology(const char *, const char *, const char *);
int cv_identify_regions(const char *, const char *);
int cv_feature_extraction_single(const char *, const char *);
int cv_feature_extraction_multi(const char *, const char *);
int cv_feature_extraction_test(const char *, const char *, const char *);
int cv_feature_extraction(const char *, const char *, const char *, const char *);

#endif /* COMPUTER_VISION_H_ */
