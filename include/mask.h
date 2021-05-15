/**
 * \file
 *	Masking functions
 *
 * \author
 *	Kadir Yanık <kdrynkk@gmail.com>
 */

#ifndef MASK_H_
#define MASK_H_

#include <stdint.h>

#include "util.h"

/*------------------------------------------------------------------------------*/
typedef struct {
    int16_t *buf;
    uint32_t width;
    uint32_t height;
} mask_t;

#define sfree_mask(_mask) do {	\
	if (_mask) {		\
	    sfree(_mask->buf);	\
	    sfree(_mask);	\
	}			\
    } while (0)

/*------------------------------------------------------------------------------*/
mask_t* mask_read_from_file(const char *);
int mask_apply(image_t, mask_t);

#endif /* MASK_H_ */
