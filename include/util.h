/**
 * \file
 *	Utilities
 *
 * \author
 *	Kadir Yanık <kdrynkk@gmail.com>
 */

#ifndef UTIL_H_
#define UTIL_H_

#include <stdint.h>

/*------------------------------------------------------------------------------*/
#define sfree(_p) do {	    \
	if ((_p)) {	    \
	    free((_p));	    \
	    (_p) = NULL;    \
	}		    \
    } while (0)

/*------------------------------------------------------------------------------*/
/* Goto fail in error with expression */
#define util_fiee(f,e) do { \
	if ((f) != 0) {	    \
	    e;		    \
	    goto fail;	    \
	}		    \
    } while (0)

#define util_fie(f) util_fiee(f,{ do { } while (0); })

/*------------------------------------------------------------------------------*/
/* Goto fail/success in condition true with expression */
#define util_xite(c,l,e) do {	\
	if (c) {		\
	    e;			\
	    goto l;		\
	}			\
    } while (0)

/* fail in true w/o expression */
#define util_fit(c) util_xite(c,fail,{ do { } while (0); })
#define util_fite(c,e) util_xite(c,fail,e)
/* success in true w/o expression */
#define util_sit(c) util_xite(c,success,{ do { } while (0); })
#define util_site(c,e) util_xite(c,success,e)

/*------------------------------------------------------------------------------*/
typedef struct {
    uint8_t *buf;	/* pixels */
    uint8_t cb;		/* colour bytes */
    uint32_t width;	/* image width */
    uint32_t height;	/* image height */
    uint32_t size;	/* size for allocation */
} image_t;

#define sfree_image(_image) do {    \
	if (_image) {		    \
	    sfree(_image->buf);	    \
	    sfree(_image);	    \
	}			    \
    } while (0)

/*------------------------------------------------------------------------------*/
struct str_node {
    char *str;		    /* data */
    struct str_node *next;  /* next str-node pointer */
};
typedef struct str_node str_node_t;

str_node_t* util_sl_insert(str_node_t **, char *);
void util_sl_free(str_node_t **);

/*------------------------------------------------------------------------------*/
int plot_histogram(const uint32_t* const);

#endif /* UTIL_H_ */
