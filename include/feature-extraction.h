/**
 * \file
 *	Feature extraction functions
 *
 * \author
 *	Kadir Yanık <kdrynkk@gmail.com>
 */

#ifndef FEATURE_EXT_H_
#define FEATURE_EXT_H_

#include <stdint.h>

#include "util.h"
#include "draw.h"

/*------------------------------------------------------------------------------*/
typedef struct {
    uint8_t noe;
    double *feature;
} features_t;

#define sfree_features(_features) do {	\
	if (_features) {		\
	    sfree(_features->feature);	\
	    sfree(_features);		\
	}				\
    } while (0)

/*------------------------------------------------------------------------------*/
features_t* fe_get_avg(image_t, regions_t);
int fe_save(const char *, features_t);

#endif /* FEATURE_EXT_H_ */
