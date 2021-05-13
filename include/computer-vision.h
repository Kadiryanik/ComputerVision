#ifndef COMPUTER_VISION_H_
#define COMPUTER_VISION_H_

#include "bmp.h"

/*------------------------------------------------------------------------------*/
int test_bmp_file(const char *filename);
int convert_binary(const char *filename);
int convert_grayscale(const char *filename);
int draw_tests(const char *filename, plus_t plus, rectangle_t rect,
	circle_t circle, ellipse_t ellipse);

#endif /* COMPUTER_VISION_H_ */
