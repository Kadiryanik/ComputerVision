/**
 * \file
 *	Moment functions
 *
 * \author
 *	Kadir Yanık <kdrynkk@gmail.com>
 */

#ifndef MOMENT_H_
#define MOMENT_H_

#include <stdint.h>

#include "util.h"
#include "draw.h"

/*------------------------------------------------------------------------------*/
double moment_normalized_central(image_t, region_t, uint8_t, uint8_t);

#endif /* MOMENT_H_ */
