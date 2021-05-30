/**
 * \file
 *	Feature extraction functions
 *
 * \author
 *	Kadir Yanık <kdrynkk@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "log.h"
#include "util.h"
#include "bmp.h"
#include "morphology.h"
#include "moment.h"
#include "feature-extraction.h"

#ifndef LOG_LEVEL_CONF_FE
#define LOG_LEVEL LOG_LEVEL_ERR
#else /* LOG_LEVEL_CONF_FE */
#define LOG_LEVEL LOG_LEVEL_CONF_FE
#endif /* LOG_LEVEL_CONF_FE */

/*------------------------------------------------------------------------------*/
static features_t* _fe_get(image_t image, region_t region)
{
    uint8_t i = 0;
    features_t *features = NULL;

    util_fite(((features = (features_t *)calloc(1, sizeof(features_t))) == NULL),
	    LOG_ERR("Features allocation failed!\n"));
    features->noe = 7;

    util_fite(((features->feature = (double *)calloc(features->noe, sizeof(double))) == NULL),
	    LOG_ERR("Features->feature allocation failed!\n"));

    features->feature[0] = moment_normalized_central(image, region, 2, 0)
	+ moment_normalized_central(image, region, 0, 2);

    features->feature[1] = pow(moment_normalized_central(image, region, 2, 0)
	    - moment_normalized_central(image, region, 0, 2), 2)
	+ (moment_normalized_central(image, region, 1, 1) * 4);

    features->feature[2] = pow(moment_normalized_central(image, region, 3, 0)
	    - (3 * moment_normalized_central(image, region, 1, 2)), 2)
	+ pow((3 * moment_normalized_central(image, region, 2, 1))
		- moment_normalized_central(image, region, 0, 3), 2);

    features->feature[3] = pow(moment_normalized_central(image, region, 3, 0)
	    + moment_normalized_central(image, region, 1, 2), 2)
	+ pow(moment_normalized_central(image, region, 2, 1)
		+ moment_normalized_central(image, region, 0, 3), 2);

    features->feature[4] = ((moment_normalized_central(image, region, 3, 0)
		- (3 * moment_normalized_central(image, region, 1, 2)))
	    * (moment_normalized_central(image, region, 3, 0)
		+ moment_normalized_central(image, region, 1, 2))
	    * ( (pow(moment_normalized_central(image, region, 3, 0)
			+ moment_normalized_central(image, region, 1, 2), 2))
		- ( 3 * pow(moment_normalized_central(image, region, 2, 1)
			+ moment_normalized_central(image, region, 0, 3), 2)) ))
	+ ((3 * moment_normalized_central(image, region, 2, 1)
		    - moment_normalized_central(image, region, 0, 3))
		* (moment_normalized_central(image, region, 2, 1)
		    + moment_normalized_central(image, region, 0, 3))
		* ( (3 * pow(moment_normalized_central(image, region, 3, 0)
			    + moment_normalized_central(image, region, 1, 2), 2))
		    - pow(moment_normalized_central(image, region, 2, 1)
			+ moment_normalized_central(image, region, 0, 3), 2) ));

    features->feature[5] = ( (moment_normalized_central(image, region, 2, 0)
		- moment_normalized_central(image, region, 0, 2))
	    * (pow(moment_normalized_central(image, region, 3, 0)
		    + moment_normalized_central(image, region, 1, 2), 2)
		- pow(moment_normalized_central(image, region, 2, 1)
		    + moment_normalized_central(image, region, 0, 3), 2)) )
	+ 4 * moment_normalized_central(image, region, 1, 1)
	* (moment_normalized_central(image, region, 3, 0)
		+ moment_normalized_central(image, region, 1, 2))
	* (moment_normalized_central(image, region, 2, 1)
		+ moment_normalized_central(image, region, 0, 3));

    features->feature[6] = ((3 * moment_normalized_central(image, region, 2, 1)
		- moment_normalized_central(image, region, 0, 3))
	    * (moment_normalized_central(image, region, 3, 0)
		+ moment_normalized_central(image, region, 1, 2))
	    * ( pow(moment_normalized_central(image, region, 3, 0)
		    + moment_normalized_central(image, region, 1, 2), 2)
		- (3 * pow(moment_normalized_central(image, region, 2, 1)
			+ moment_normalized_central(image, region, 0, 3), 2))))
	- (moment_normalized_central(image, region, 3, 0)
		- 3 * moment_normalized_central(image, region, 1, 2)
		* (moment_normalized_central(image, region, 2, 1)
		    + moment_normalized_central(image, region, 0, 3))
		* (3 * pow(moment_normalized_central(image, region, 3, 0)
			+ moment_normalized_central(image, region, 1, 2), 2)
		    - pow(moment_normalized_central(image, region, 2, 1)
			+ moment_normalized_central(image, region, 0, 3), 2)));

    LOG_DBG("Region %u: [%d,%d_%d,%d]\n", region.label, region.rect.x,
	    region.rect.y, region.rect.width, region.rect.height);
    for (i = 0; i < features->noe; i++) {
	LOG_DBG("\tfeature[%u] = %f\n", i, features->feature[i]);
    }

    goto success;

fail:
    sfree_features(features);

success:
    return features;
}

/*------------------------------------------------------------------------------*/
features_t* fe_get_avg(image_t image, regions_t regions)
{
    int i = 0, j = 0;
    features_t *features = NULL, *temp_features = NULL;

    for (i = 0; i < regions.noe; i++) {
	if (i == 0) {
	    /* initialize features */
	    util_fit(((features = _fe_get(image, regions.region[i])) == NULL));
	} else {
	    /* read into temp_features and add into features */
	    util_fit(((temp_features = _fe_get(image, regions.region[i])) == NULL));
	    for (j = 0; j < features->noe; j++) {
		features->feature[j] += temp_features->feature[j];
	    }
	    sfree_features(temp_features);
	}
    }

    /* Calculate avg and return */
    for (i = 0; i < features->noe; i++) {
	features->feature[i] /= regions.noe;
    }

    goto success;

fail:
    LOG_ERR("%s failed!\n", __func__);
    sfree_features(features);
    sfree_features(temp_features);

success:
    return features;
}

/*------------------------------------------------------------------------------*/
int fe_save(const char *filename, features_t features)
{
    FILE *file = NULL;
    int i = 0, ret = 0;

    LOG_DBG("filename:'%s' features:%p\n", filename, &features);

    util_fite(((file = fopen(filename, "w")) == NULL),
	    LOG_ERR("File open failed!\n"));

    /* Write noe first */
    util_fite((fprintf(file, "%u\n", features.noe) < 0), LOG_ERR("fprintf failed\n"));

    /* Get avg and write result to file */
    for (i = 0; i < features.noe; i++) {
	util_fite((fprintf(file, "%f\n", features.feature[i]) < 0),
		LOG_ERR("fprintf failed\n"));
    }

    /* End with EOF */
    util_fite((fprintf(file, "EOF\n") < 0), LOG_ERR("fprintf failed\n"));

    goto success;

fail:
    ret = -1;

success:
    if (file) fclose(file);
    return ret;
}

