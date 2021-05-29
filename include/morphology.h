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
#include "draw.h"

/*------------------------------------------------------------------------------*/
typedef struct {
    int16_t old_label;	/* first calculated label id */
    uint8_t label;	/* final label id */
    rectangle_t rect;	/* region frame info */
} region_t;

/*------------------------------------------------------------------------------*/
typedef struct {
    uint8_t noe;	/* number of entities */
    region_t *region;	/* regions */
} regions_t;

/*------------------------------------------------------------------------------*/
int morp_apply_dilation(image_t);
int morp_apply_erosion(image_t);
int morp_apply_open(image_t);
int morp_apply_close(image_t);
int morp_apply(image_t, const char *);
image_t* morp_identify_regions(image_t, regions_t *);

#endif /* MORPHOLOGY_H_ */
