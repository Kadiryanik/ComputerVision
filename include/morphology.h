/**
 * \file
 *	Morphology functions
 *
 * \author
 *	Kadir Yanık <kdrynkk@gmail.com>
 */

#ifndef MORPHOLOGY_H_
#define MORPHOLOGY_H_

#include "util.h"

/*------------------------------------------------------------------------------*/
int morp_apply_dilation(image_t);
int morp_apply_erosion(image_t);
int morp_apply_open(image_t);
int morp_apply_close(image_t);
int morp_apply(image_t, const char *);

#endif /* MORPHOLOGY_H_ */
