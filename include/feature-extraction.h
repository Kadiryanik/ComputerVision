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
    uint32_t total_noe;	/* how many regions contain that we calculated the avg */
    uint8_t noe;	/* feature array noe */
    double *feature;
} features_t;

#define sfree_features(_features) do {	\
	if (_features) {		\
	    sfree(_features->feature);	\
	    sfree(_features);		\
	}				\
    } while (0)

/*------------------------------------------------------------------------------*/
struct _class {
    uint8_t index;	    /* used for holding index */
    char *name;		    /* uniqe class identifier */
    str_node_t *files;	    /* input image file names */
    features_t *features;   /* calculated features */
    struct _class *next;    /* next class pointer */
};
typedef struct _class class_t;

class_t* fe_classes_insert(class_t **, char *, features_t *);
int fe_classes_update(class_t *, image_t, regions_t);
void fe_classes_free(class_t **);

/*------------------------------------------------------------------------------*/
features_t* fe_get_avg(image_t, regions_t);
int fe_test(image_t, regions_t, class_t, image_t);
int fe_save(const char *, features_t);
class_t* fe_load_classes_with_features(const char *);
class_t* fe_load_classes(const char *);
int fe_save_classes(const char *, class_t *);

#endif /* FEATURE_EXT_H_ */
