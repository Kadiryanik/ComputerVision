/**
 * \file
 *	Feature extraction functions
 *
 * \author
 *	Kadir Yanık <kdrynkk@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

#define FILLED_RECT_SIZE    10
#define FSCANF_READ_BUFLEN  256

/*------------------------------------------------------------------------------*/
extern double fe_match_epsilon;  /* defined in test.c */

/*------------------------------------------------------------------------------*/
static features_t* _fe_get(image_t image, region_t region)
{
    uint8_t i = 0;
    features_t *features = NULL;

    util_fite(((features = (features_t *)calloc(1, sizeof(features_t))) == NULL),
	    LOG_ERR("Features allocation failed!\n"));
    features->noe = SUPPORTED_FEATURES_NOE;

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
static features_t* _fe_get_sum(image_t image, regions_t regions)
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
    features->total_noe = regions.noe;

    goto success;

fail:
    LOG_ERR("%s failed!\n", __func__);
    sfree_features(features);
    sfree_features(temp_features);

success:
    return features;
}

/*------------------------------------------------------------------------------*/
class_t* fe_classes_insert(class_t **head, char *name, features_t *features)
{
    class_t *ptr = *head;
    uint8_t i = 0;

    if (*head == NULL) {
	util_fite(((*head = (class_t *)calloc(1, sizeof(class_t))) == NULL),
		LOG_ERR("Class first allocation failed\n"));
	ptr = *head;
    } else {
	while (ptr->next != NULL) {
	    ptr = ptr->next;
	}
	util_fite(((ptr->next = (class_t *)calloc(1, sizeof(class_t))) == NULL),
		LOG_ERR("Class new allocation failed\n"));
	ptr = ptr->next;
    }
    util_fite(((ptr->name = strdup(name)) == NULL),
	    LOG_ERR("Duplicating class name failed\n"));
    if (features != NULL) {
	util_fite(((ptr->features = (features_t *)calloc(1, sizeof(features_t))) == NULL),
		LOG_ERR("Features allocation failed!\n"));
	ptr->features->total_noe = features->total_noe;
	ptr->features->noe = features->noe;
	util_fite(((ptr->features->feature = (double *)calloc(features->noe, sizeof(double))) == NULL),
		LOG_ERR("Features->feature allocation failed!\n"));
	for (i = 0; i < features->noe; i++) {
	    ptr->features->feature[i] = features->feature[i];
	}
    }
    LOG_DBG("Class %p - '%s' added\n", ptr, name);

    goto success;

fail:
    if (ptr) sfree(ptr->name);
    if (ptr == *head) {
	sfree(*head);
	ptr = NULL;
    } else {
	sfree(ptr);
    }

success:
    return ptr;
}

/*------------------------------------------------------------------------------*/
int fe_classes_update(class_t *_class, image_t image, regions_t regions)
{
    int ret = 0, i = 0;
    features_t *features = NULL;

    if (_class->features == NULL) {
	util_fit(((_class->features = fe_get_avg(image, regions)) == NULL));
    } else {
	util_fit(((features = _fe_get_sum(image, regions)) == NULL));
	/* Calculate new avg */
	for (i = 0; i < _class->features->noe; i++) {
	    _class->features->feature[i] =
		((_class->features->feature[i] * _class->features->total_noe) +
		    (features->feature[i])) /
		(_class->features->total_noe + features->total_noe);
	    _class->features->total_noe += features->total_noe;
	}
    }

    goto success;

fail:
    LOG_ERR("%s failed!\n", __func__);
    ret = -1;

success:
    sfree_features(features);
    return ret;
}

/*------------------------------------------------------------------------------*/
void fe_classes_free(class_t **head)
{
    class_t *ptr = *head, *current = NULL;

    while (ptr != NULL) {
	sfree(ptr->name);
	util_sl_free(&(ptr->files));
	sfree_features(ptr->features);

	current = ptr;
	ptr = ptr->next;
	sfree(current);
    }
    *head = NULL;
}

/*------------------------------------------------------------------------------*/
features_t* fe_get_avg(image_t image, regions_t regions)
{
    int i = 0;
    features_t *features = NULL;

    util_fit(((features = _fe_get_sum(image, regions)) == NULL));

    /* Calculate avg and return */
    for (i = 0; i < features->noe; i++) {
	features->feature[i] /= features->total_noe;
    }

    goto success;

fail:
    LOG_ERR("%s failed!\n", __func__);
    sfree_features(features);

success:
    return features;
}

/*------------------------------------------------------------------------------*/
int fe_test(image_t image, regions_t regions, class_t classes, image_t final_image)
{
    int i = 0, j = 0, ret = 0, class_count = 0, matched_class_index = 0;
    uint8_t *matched_classes = NULL, max = 0, identified = 0;
    features_t *features = NULL;
    class_t *current_class = NULL;
    double val = 0, min = 0;
    rectangle_t rect = { .x = 0, .y = 0, .height = FILLED_RECT_SIZE,
	.width = FILLED_RECT_SIZE };

    current_class = &classes;
    while (current_class != NULL) {
	class_count++;
	current_class = current_class->next;
    }

    util_fit(((matched_classes = (uint8_t *)calloc(1, class_count * sizeof(uint8_t))) == NULL));

    for (i = 0; i < regions.noe; i++) {
	util_fit(((features = _fe_get(image, regions.region[i])) == NULL));

	identified = 0;
	for (j = 0; j < features->noe; j++) {
	    /* initial to first class */
	    min = fabs(classes.features->feature[j] - features->feature[j]);
	    matched_class_index = 0;

	    /* check for other classes */
	    current_class = classes.next;
	    while (current_class != NULL) {
		val = fabs(current_class->features->feature[j] - features->feature[j]);
		if (val < min) {
		    min = val;
		    matched_class_index = current_class->index;
		}
		current_class = current_class->next;
	    }

	    if (min < fe_match_epsilon) {
		matched_classes[matched_class_index]++;
		identified = 1;
	    }
	}

	if (!identified) continue;

	matched_class_index = 0;
	max = matched_classes[matched_class_index];
	matched_classes[matched_class_index] = 0;

	current_class = classes.next;
	while (current_class != NULL) {
	    if (max < matched_classes[current_class->index]) {
		matched_class_index = current_class->index;
		max = matched_classes[matched_class_index];
	    }
	    /* also clear the array in this loop */
	    matched_classes[current_class->index] = 0;
	    current_class = current_class->next;
	}
	/* Draw a rectangle around the region with class color */
	draw_rect(final_image, regions.region[i].rect, 0, matched_class_index);

	/* Draw a filled rectangle to the left-top corner with class color */
	rect.x = regions.region[i].rect.x;
	rect.y = regions.region[i].rect.y;
	draw_filled_rect(final_image, rect, 0, matched_class_index);

	/* TODO: Allow user to define class colors with formatted input files.
	 *       Add color-class relation into image corner.
	 *       Add percentage into region corner. */

	sfree_features(features);
    }

    goto success;

fail:
    LOG_ERR("%s failed!\n", __func__);
    ret = -1;
    sfree_features(features);

success:
    sfree(matched_classes);
    return ret;
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

    /* Write result to file */
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

/*------------------------------------------------------------------------------*/
class_t* fe_load_classes_with_features(const char *filename)
{
    FILE *file = NULL;
    /* Points to classes head pointer and store all classes */
    class_t *classes = NULL, *current_class = NULL;
    char buf[FSCANF_READ_BUFLEN];
    uint8_t i = 0, class_index = 0;
    features_t features = { .total_noe = 0, .noe = 0, .feature = NULL };

    LOG_DBG("filename:'%s'\n", filename);

    util_fite(((file = fopen(filename, "r")) == NULL),
	    LOG_ERR("File open failed!\n"));

    do {
	util_fite((fscanf(file, "%s", buf) < 0), LOG_ERR("Reading file failed\n"));
	if (strcmp(buf, "CLASS") == 0) {
	    features.total_noe = features.noe = 0;
	    util_fite((fscanf(file, "%s %u %hhu", buf, &features.total_noe, &features.noe) < 0),
		    LOG_ERR("Reading class name and features numbers\n"));
	    util_fite((features.noe != SUPPORTED_FEATURES_NOE),
		    LOG_ERR("Unsupported features number of entries. (%u != %u)\n",
			features.noe, SUPPORTED_FEATURES_NOE));

	    util_fite(((features.feature = (double *)calloc(features.noe, sizeof(double))) == NULL),
		    LOG_ERR("Features->feature allocation failed!\n"));
	    for (i = 0; i < features.noe; i++) {
		util_fite((fscanf(file, "%lf", &features.feature[i]) < 0),
			LOG_ERR("Reading feature[%u] failed\n", i));
	    }

	    util_fit(((current_class = fe_classes_insert(&classes, buf, &features)) == NULL));

	    sfree(features.feature);
	    current_class->index = class_index++;
	} else if (strcmp(buf, "EOF") == 0) {
	    break;
	}
    } while (1); /* Reading EOF or fscanf fail will break the loop*/

    goto success;

fail:
    fe_classes_free(&classes);
    sfree(features.feature);

success:
    if (file) fclose(file);
    return classes;
}

/*------------------------------------------------------------------------------*/
class_t* fe_load_classes(const char *filename)
{
    FILE *file = NULL;
    /* Points to classes head pointer and store all classes */
    class_t *classes = NULL, *current_class = NULL;
    char buf[FSCANF_READ_BUFLEN];
    uint8_t class_index = 0;

    LOG_DBG("filename:'%s'\n", filename);

    util_fite(((file = fopen(filename, "r")) == NULL),
	    LOG_ERR("File open failed!\n"));

    do {
	util_fite((fscanf(file, "%s", buf) < 0), LOG_ERR("Reading file failed\n"));
	if (strcmp(buf, "CLASS") == 0) {
	    util_fite((fscanf(file, "%s", buf) < 0), LOG_ERR("Reading class name failed\n"));
	    util_fit(((current_class = fe_classes_insert(&classes, buf, NULL)) == NULL));
	    current_class->index = class_index++;
	} else if (strcmp(buf, "EOF") == 0) {
	    break;
	} else {
	    util_fite((current_class == NULL),
		    LOG_ERR("Current class is NULL, invalid file format\n"));
	    util_fit((util_sl_insert(&current_class->files, buf) == NULL));
	}
    } while (1); /* Reading EOF or fscanf fail will break the loop*/

    goto success;

fail:
    fe_classes_free(&classes);

success:
    if (file) fclose(file);
    return classes;
}

/*------------------------------------------------------------------------------*/
int fe_save_classes(const char *filename, class_t *_class)
{
    FILE *file = NULL;
    int i = 0, ret = 0;
    class_t *current_class = _class;

    LOG_DBG("filename:'%s' class:%p\n", filename, current_class);

    util_fite(((file = fopen(filename, "w")) == NULL),
	    LOG_ERR("File open failed!\n"));

    while (current_class != NULL) {
	util_fite((fprintf(file, "CLASS %s %u %u\n", current_class->name,
		    current_class->features->total_noe,
		    current_class->features->noe) < 0), LOG_ERR("fprintf failed\n"));

	/* Write result to file */
	for (i = 0; i < current_class->features->noe; i++) {
	    util_fite((fprintf(file, "%f\n", current_class->features->feature[i]) < 0),
		    LOG_ERR("fprintf failed\n"));
	}

	current_class = current_class->next;
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

